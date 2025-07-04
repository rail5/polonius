/**
 * Copyright (C) 2025 rail5
 */

#ifndef SRC_EDIT_EXPRESSION_H_
#define SRC_EDIT_EXPRESSION_H_

#include "../polonius.h"
#include "../block.h"

#include <deque>
#include <string>
#include <cstdint>

namespace Polonius::Editor {
/**
 * @class Expression
 * @brief Represents a mathematical expression with Blocks as its operands.
 * 
 * The mathematics of Blocks is explained in https://github.com/rail5/polonius/wiki/Instruction-Optimization
 * 
 * By chaining Blocks together in expressions, we can represent Polonius instruction sequences in a way that
 * allows for efficient optimization according to the theorems presented in the above-linked document.
 * 
 * The expression abstractly takes the form:
 * 		<block> <operator> <block> <operator> <block> ...
 * Where <operator> is one of:
 * 		+ (addition) [insert operations]
 * 		- (subtraction) [remove operations]
 * 		* (multiplication) [replace operations]
 * 
 * The expression is evaluated left-to-right, with the first operator being applied to the first two blocks,
 * and the result being used as the first operand for the next operator.
 * There is no operator precedence, so all operators are evaluated in the order they appear.
 * 
 * When appending a new block (and operator) to the expression, the expression is evaluated immediately
 * 	according to the rules of the operator.
 */
class Expression {
	private:
		std::deque<Block> blocks;
		uint8_t optimization_level = 2;

		void re_evaluate();

		void _insert(Block&& block);
		void _remove(Block&& block);
	public:
		Expression();
		explicit Expression(uint8_t optimization_level);

		void insert(Block&& block);
		void remove(Block&& block);
		void replace(Block&& block);

		void add_term(Polonius::InstructionType op, uint64_t start, const std::string& text);
		void add_term(Polonius::InstructionType op, uint64_t start, uint64_t end);

		// Operator overloads
		Expression operator+(Block&& block);
		Expression operator-(Block&& block);
		Expression operator*(Block&& block);

		void set_optimization_level(uint8_t level);

		bool empty() const;

		void clear();

		auto begin() {
			return blocks.begin();
		}

		auto end() {
			return blocks.end();
		}

		auto cbegin() const {
			return blocks.cbegin();
		}

		auto cend() const {
			return blocks.cend();
		}
};
} // namespace Polonius::Editor

#endif // SRC_EDIT_EXPRESSION_H_
