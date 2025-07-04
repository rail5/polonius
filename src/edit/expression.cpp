/**
 * Copyright (C) 2025 rail5
 */

#include "expression.h"

Polonius::Editor::Expression::Expression() = default;

Polonius::Editor::Expression::Expression(uint8_t optimization_level) {
	this->optimization_level = optimization_level;
}


void Polonius::Editor::Expression::set_optimization_level(uint8_t level) {
	optimization_level = level;

	// If we change the optimization level, we need to re-evaluate the expression
	// If we don't do this, the expression will not be optimized correctly when we add more terms
	// (And will become, in fact, an incorrect and potentially invalid expression)
	// E.g, if we've *been* optimizing according to -O1,
	// And we now set the optimization level to -O2,
	// We need to re-evaluate the expression to apply the new optimizations from the beginning
	re_evaluate();
}

void Polonius::Editor::Expression::re_evaluate() {
	// Re-evaluate the entire expression

	std::deque<Polonius::Block> blocks_copy = std::move(blocks);
	blocks.clear();

	for (auto& block : blocks_copy) {
		switch (block.get_operator()) {
			case Polonius::InstructionType::INSERT:
				_insert(std::move(block));
				break;
			case Polonius::InstructionType::REMOVE:
				_remove(std::move(block));
				break;
			case Polonius::InstructionType::REPLACE:
				replace(std::move(block));
				break;
		}
	}
}

void Polonius::Editor::Expression::insert(Polonius::Block&& block) {
	_insert(std::move(block));
	re_evaluate(); // Guarantees the expression will remain sorted
}

void Polonius::Editor::Expression::remove(Polonius::Block&& block) {
	_remove(std::move(block));
	re_evaluate(); // Guarantees the expression will remain sorted
}

void Polonius::Editor::Expression::_insert(Polonius::Block&& block) {
	block.set_operator(Polonius::InstructionType::INSERT);
	std::deque<Polonius::Block> inserts_before_this_instruction;
	std::deque<Polonius::Block> inserts_after_this_instruction;
	std::deque<Polonius::Block> removes;
	std::deque<Polonius::Block> replaces;
	uint64_t left_shift = 0;
	switch (optimization_level) {
		default:
			[[fallthrough]];
		case 3:
			[[fallthrough]];
		case 2:
			// Level 2 optimizations:
			// Apply theorem #3 (eliminating redundant insert/remove pairs)
			
			// Iterate backwards through the blocks
			while (!blocks.empty() && blocks.back().get_operator() != Polonius::InstructionType::INSERT) {
				Polonius::Block* last = &blocks.back();
				switch (last->get_operator()) {
					case Polonius::InstructionType::INSERT:
						throw std::runtime_error("Unexpected INSERT block in the middle of the expression.");
					case Polonius::InstructionType::REPLACE:
						replaces.emplace_front(std::move(*last));
						blocks.pop_back();
						break;
					case Polonius::InstructionType::REMOVE:
						if (last->start() < block.start() + left_shift) {
							left_shift += last->size(); // Track position shifts
						} else if (last->start() == block.start() + left_shift) {
							// Yes, they have to be EXACTLY EQUAL in order for there to be any redundancy.

							// There is a redundancy between the remove and this insert
							// Ie, we're removing some characters and then inserting to the same position
							// This can be simplified to a single replace
							Polonius::BlockOverlap overlap = last->overlap(block.start() + left_shift, block.end() + left_shift);

							// Simplify these two instructions to a single replace
							Polonius::Block replace_block = block;
							replace_block.remove(overlap.end - left_shift + 1, block.end());
							replace_block.set_operator(Polonius::InstructionType::REPLACE);

							// Store the block's original start position
							uint64_t original_start = block.start();

							// Remove the overlap from both instructions
							last->remove(overlap.start, overlap.end);
							block.remove(overlap.start - left_shift, overlap.end - left_shift);

							// After removing the overlap, either:
							// 1. There is *only* redundancy (nothing left over after the end of the overlap)
							// 2. Some of the INSERT is left over after the overlap
							// 3. Some of the REMOVE is left over
							if (last->empty()) {
								blocks.pop_back();
							}

							// Update all of the blocks in-between the redundant remove/insert pair
							for (auto& b : removes) {
								if (b.start() >= original_start + left_shift) {
									b.shift_right(overlap.end - overlap.start + 1);
								} else {
									left_shift -= b.size(); // This will undo each position shift one-by-one in reverse order
											// Ie, left_shift == 0 at the end of this loop
								}
								blocks.emplace_back(std::move(b));
							}
							removes.clear();
							for (auto& b : replaces) {
								Polonius::BlockOverlap ov = b.overlap(original_start + left_shift, UINT64_MAX);
								if (!ov.empty) {
									Polonius::Block before_overlap = b;
									Polonius::Block from_overlap_start_to_the_end = b;
									before_overlap.remove(ov.start, before_overlap.end());
									if (ov.start > 0) {
										from_overlap_start_to_the_end.remove(from_overlap_start_to_the_end.start(), ov.start - 1);
									}

									if (!before_overlap.empty()) {
										blocks.emplace_back(std::move(before_overlap));
									}
									if (!from_overlap_start_to_the_end.empty()) {
										from_overlap_start_to_the_end.shift_right(overlap.end - overlap.start + 1);
										blocks.emplace_back(std::move(from_overlap_start_to_the_end));
									}
								} else {
									blocks.emplace_back(std::move(b));
								}
							}
							replaces.clear();
							
							// Add our replacement REPLACE block
							blocks.emplace_back(std::move(replace_block));
							
							// If the INSERT block is now empty, just return
							if (block.empty()) {
								return;
							}
							// Otherwise, fall through to the level 1 optimizations
							break;
						}
						removes.emplace_front(std::move(*last));
						blocks.pop_back();
						break;
				}
			}
			// Add back all the removes and replaces if we've made it this far
			for (auto& b : removes) {
				blocks.emplace_back(std::move(b));
			}
			removes.clear();
			for (auto& b : replaces) {
				blocks.emplace_back(std::move(b));
			}
			replaces.clear();
			[[fallthrough]];
		case 1:
			// Level 1 optimizations:
			// Apply theorem #0 (combining insert instructions)
			// We don't need to actually strictly *combine* them,
			// We just need to put all the insert instructions next to each other
			// So that we can execute all of them on a single pass-through

			// The instruction sequence at this moment will look like this:
			// {ALL OF THE INSERT INSTRUCTIONS}
			// {ALL OF THE REMOVE INSTRUCTIONS}
			// {ALL OF THE REPLACE INSTRUCTIONS}
			// {This INSERT instruction}
			// So let's move this one on up to the INSERT instructions
			while (!blocks.empty()) {
				Polonius::Block* last = &blocks.back();
				Polonius::BlockOverlap overlap;
				switch (last->get_operator()) {
					case Polonius::InstructionType::INSERT:
						// Make sure the INSERT instructions are always sorted
						if (last->start() > block.start()) {
							last->shift_right(block.size());
							inserts_after_this_instruction.emplace_front(std::move(*last));
						} else {
							// If there's an overlap between these two blocks,
							// They must be combined into a single block in order to
							// be executed on a single pass of the file
							// E.g, 'insert 0 abc' followed by 'insert 1 x'
							// *must* become 'insert 0 axbc'
							Polonius::Block combined = Polonius::combine_inserts(*last, block);
							if (!combined.empty()) {
								block = std::move(combined);
							} else {
								inserts_before_this_instruction.emplace_front(std::move(*last));
							}
						}
						blocks.pop_back();
						break;
					case Polonius::InstructionType::REMOVE:
						if (last->start() <= block.start()) {
							block.shift_right(last->size());
						} else {
							last->shift_right(block.size());
						}
						removes.emplace_front(std::move(*last));
						blocks.pop_back();
						break;
					case Polonius::InstructionType::REPLACE:
						overlap = last->overlap(block);
						if (!overlap.empty) {
							Polonius::Block pre_overlap = *last;
							Polonius::Block post_overlap = *last;
							pre_overlap.remove(overlap.start, last->end());

							// In the event that the overlap starts at the beginning of the REPLACE block,
							// Calling remove(start, start - 1) can be disasterous
							// E.g, if the block starts at '0', this will underflow and clear the entire block
							// And even if the block doesn't start at 0, we'll end up removing some of what we want to keep
							// Of course, what we want to keep in post_overlap is the entire portion of the block from
							// where the overlap starts to the end
							if (overlap.start != last->start()) {
								post_overlap.remove(last->start(), overlap.start - 1);
							}
							blocks.pop_back();
							if (!pre_overlap.empty()) {
								replaces.emplace_front(std::move(pre_overlap));
							}
							if (!post_overlap.empty()) {
								post_overlap.shift_right(block.size());
								replaces.emplace_front(std::move(post_overlap));
							}
						} else if (last->start() >= block.start()) {
							last->shift_right(block.size());
							replaces.emplace_front(std::move(*last));
							blocks.pop_back();
						} else {
							replaces.emplace_front(std::move(*last));
							blocks.pop_back();
						}
						break;
				}
			}
			for (auto& insert : inserts_before_this_instruction) {
				blocks.emplace_back(std::move(insert));
			}
			[[fallthrough]];
		case 0:
			if (!block.empty()) {
				blocks.emplace_back(std::move(block));
			}
			break;
	}
	// Re-add the old inserts, removes, and replaces
	for (auto& insert : inserts_after_this_instruction) {
		blocks.emplace_back(std::move(insert));
	}
	for (auto& remove : removes) {
		blocks.emplace_back(std::move(remove));
	}
	for (auto& replace : replaces) {
		blocks.emplace_back(std::move(replace));
	}
}

void Polonius::Editor::Expression::_remove(Polonius::Block&& block) {
	block.set_operator(Polonius::InstructionType::REMOVE);
	std::deque<Polonius::Block> inserts;
	std::deque<Polonius::Block> removes_before_this_instruction;
	std::deque<Polonius::Block> removes_after_this_instruction;
	std::deque<Polonius::Block> replaces;
	Polonius::BlockOverlap overlap;
	uint64_t left_shift = 0;
	uint64_t right_shift = 0;
	Polonius::Block recursive_process;
	switch (optimization_level) {
		default:
			[[fallthrough]];
		case 3:
			[[fallthrough]];
		case 2:
			// Level 2 optimizations:
			// Apply theorem #4 (eliminating redundant insert/remove pairs)
			while (!blocks.empty() && !block.empty()) {
				Polonius::Block* last = &blocks.back();
				switch (last->get_operator()) {
					case Polonius::InstructionType::INSERT:
						overlap = last->overlap(block.start() + left_shift - right_shift, block.end() + left_shift - right_shift);
						if (!overlap.empty) {
							// Found a redundant pair
							Polonius::Block insert_before_redundancy = *last;
							Polonius::Block insert_after_redundancy = *last;
							Polonius::Block remove_before_redundancy = block;
							Polonius::Block remove_after_redundancy = block;

							insert_before_redundancy.remove(overlap.start, insert_before_redundancy.end());
							insert_after_redundancy.remove(insert_after_redundancy.start(), overlap.end);

							remove_before_redundancy.remove(overlap.start - left_shift + right_shift, remove_before_redundancy.end());
							remove_after_redundancy.remove(remove_after_redundancy.start(), overlap.end - left_shift + right_shift);

							uint64_t redundancy_size = overlap.end - overlap.start + 1;
							uint64_t redundancy_start = overlap.start - left_shift + right_shift;

							// ADJUST ANY NOW-MISPLACED INSERTS, REMOVES, OR REPLACES
							uint64_t counter_right_shift = right_shift;
							for (auto& b : inserts) {
								uint64_t shift_update = 0;
								if (b.start() < block.start() + left_shift - counter_right_shift) {
									shift_update = b.size();
								}

								if (b.start() >= redundancy_start + left_shift - counter_right_shift) {
									b.shift_left(redundancy_size);
								}

								counter_right_shift -= shift_update;
							}

							uint64_t counter_left_shift = left_shift;
							for (auto& b : removes_before_this_instruction) {
								uint64_t shift_update = 0;
								if (b.start() < block.start() + counter_left_shift) {
									shift_update = b.size();
								}

								if (b.start() >= redundancy_start + counter_left_shift) {
									b.shift_left(redundancy_size);
								}

								counter_left_shift -= shift_update;
							}

							for (auto& b : replaces) {
								Polonius::BlockOverlap redundant_replace_portion = b.overlap(redundancy_start, redundancy_start + redundancy_size - 1);
								uint64_t replace_original_start = b.start();
								if (!redundant_replace_portion.empty) {
									b.remove(redundant_replace_portion.start, redundant_replace_portion.end);
								}

								if (replace_original_start >= redundancy_start) {
									b.shift_left(redundancy_size);
								}
							}

							if (!insert_after_redundancy.empty()) {
								insert_after_redundancy.shift_left(redundancy_size);
								inserts.emplace_front(std::move(insert_after_redundancy));
							}

							if (!insert_before_redundancy.empty()) {
								right_shift += insert_before_redundancy.size();
								inserts.emplace_front(std::move(insert_before_redundancy));
							}

							block.clear();

							if (!remove_after_redundancy.empty()) {
								remove_after_redundancy.shift_left(redundancy_size);
								block = std::move(remove_after_redundancy);
							}

							if (!remove_before_redundancy.empty()) {
								recursive_process = std::move(remove_before_redundancy);
							}

							blocks.pop_back();

						} else {
							if (last->start() < block.start() + left_shift - right_shift) {
								right_shift += last->size();
							}
							inserts.emplace_front(std::move(*last));
							blocks.pop_back();
						}
						break;
					case Polonius::InstructionType::REMOVE:
						if (last->start() < block.start() + left_shift) {
							left_shift += last->size();
						}
						removes_before_this_instruction.emplace_front(std::move(*last));
						blocks.pop_back();
						break;
					case Polonius::InstructionType::REPLACE:
						replaces.emplace_front(std::move(*last));
						blocks.pop_back();
						break;
				}
			}

			// Re-add everything
			for (auto& b : inserts) {
				blocks.emplace_back(std::move(b));
			}
			inserts.clear();

			for (auto& b : removes_before_this_instruction) {
				blocks.emplace_back(std::move(b));
			}
			removes_before_this_instruction.clear();
			
			for (auto& b : replaces) {
				if (!b.empty()) {
					blocks.emplace_back(std::move(b));
				}
			}
			replaces.clear();

			[[fallthrough]];
		case 1:
			// Level 1 optimizations:
			// Apply theorem #1 (combining remove instructions)
			// Most of the leg-work is already handled by INSERTs (above)
			// Here, we only have to make sure we separate REMOVEs from REPLACEs

			// At this point, the instruction sequence will look like:
			// The instruction sequence at this moment will look like this:
			// {ALL OF THE INSERT INSTRUCTIONS}
			// {ALL OF THE REMOVE INSTRUCTIONS}
			// {ALL OF THE REPLACE INSTRUCTIONS}
			// {This remove instruction}
			// So let's move this one to the REMOVE section
			while (!blocks.empty() && blocks.back().get_operator() != Polonius::InstructionType::INSERT) {
				Polonius::Block* last = &blocks.back();
				switch (last->get_operator()) {
					case Polonius::InstructionType::INSERT:
						break; // This will never happen
					case Polonius::InstructionType::REMOVE:
						// Ensure our REMOVE instructions are always sorted
						{
							Polonius::Block combined = Polonius::combine_removes(*last, block);
							if (!combined.empty()) {
								block = std::move(combined);
							} else {
								// Can't be combined, can be re-ordered
								if (last->start() < block.start()) {
									removes_before_this_instruction.emplace_front(std::move(*last));
								} else {
									last->shift_left(block.size());
									removes_after_this_instruction.emplace_front(std::move(*last));
								}
							}
						}
						blocks.pop_back();
						break;
					case Polonius::InstructionType::REPLACE:
						overlap = last->overlap(block);
						if (!overlap.empty) {
							Polonius::Block pre_overlap = *last;
							Polonius::Block post_overlap = *last;
							pre_overlap.remove(overlap.start, last->end());
							post_overlap.remove(last->start(), overlap.end);

							blocks.pop_back();

							if (!pre_overlap.empty()) {
								replaces.emplace_front(std::move(pre_overlap));
							}
							if (!post_overlap.empty()) {
								post_overlap.shift_left(block.size());
								replaces.emplace_front(std::move(post_overlap));
							}
						} else if (last->start() >= block.start()) {
							last->shift_left(block.size());
							replaces.emplace_front(std::move(*last));
							blocks.pop_back();
						} else {
							replaces.emplace_front(std::move(*last));
							blocks.pop_back();
						}
						break;
				}
			}
			for (auto& b : removes_before_this_instruction) {
				blocks.emplace_back(std::move(b));
			}
			[[fallthrough]];
		case 0:
			if (!block.empty()) {
				blocks.emplace_back(std::move(block));
			}
			break;
	}
	// Re-add the REMOVEs and REPLACEs
	for (auto& b : removes_after_this_instruction) {
		blocks.emplace_back(std::move(b));
	}
	for (auto& b : replaces) {
		blocks.emplace_back(std::move(b));
	}

	if (!recursive_process.empty()) {
		this->remove(std::move(recursive_process));
	}
}

void Polonius::Editor::Expression::replace(Polonius::Block&& block) {
	block.set_operator(Polonius::InstructionType::REPLACE);
	switch (optimization_level) {
		default:
			[[fallthrough]];
		case 3:
			[[fallthrough]];
		case 2:
			[[fallthrough]];
		case 1:
			[[fallthrough]];
		case 0:
			if (!block.empty()) {
				blocks.emplace_back(std::move(block));
			}
			break;
	}
}

Polonius::Editor::Expression Polonius::Editor::Expression::operator+(Polonius::Block&& block) {
	insert(std::move(block));
	return *this;
}

Polonius::Editor::Expression Polonius::Editor::Expression::operator-(Polonius::Block&& block) {
	remove(std::move(block));
	return *this;
}

Polonius::Editor::Expression Polonius::Editor::Expression::operator*(Polonius::Block&& block) {
	replace(std::move(block));
	return *this;
}
