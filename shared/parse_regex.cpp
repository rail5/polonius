#ifndef REGEX
	#define REGEX
	#include <regex>
#endif

#ifndef VECTOR
	#define VECTOR
	#include <vector>
#endif

#ifndef FN_IS_NUMBER
	#define FN_IS_NUMBER
	#include "is_number.cpp"
#endif

#include <iostream>

std::vector<std::string> parse_regex(std::string expression) {
	/***
	I hate this with every ounce of my being.
	I can only apologize.

	vector<string> parse_regex(string expression):
		Splits a regular expression into its component parts

		Returns a vector<string> where each string is one component part of the expression, independently usable as its own expression

		NOT FINISHED
	***/

	// The vector we will return
	std::vector<std::string> parsed_expression;

	// A flag to set when we hit a backslash (\)
	bool escaped = false;

	// A flag to set when we hit a caret (^)
	bool caret = false;

	// A flag to set when we're entering multiple characters into one element of the vector
	bool multi_char_entry = false;

	bool square_brackets_open = false;

	bool curly_braces_open = false;

	bool curly_braces_received_number = false;

	bool curly_braces_received_comma = false;

	std::string curly_braces_buffer;

	int parentheses_level = 0;

	// Iterate through each character in the string
	for(char& c : expression) {
		std::string part(1, c);
		int current_index = parsed_expression.size()-1;

		if (square_brackets_open || parentheses_level > 0 || escaped || caret) {
			multi_char_entry = true;
		} else {
			multi_char_entry = false;
		}

		if (parentheses_level < 0) {
			parentheses_level = 0;
		}

		// What do we do with this character?
		switch(c) {
			case '\\':
				escaped = !escaped;

				if (multi_char_entry) {
					parsed_expression[current_index] += part;
				} else {
					parsed_expression.push_back(part);
				}

				caret = false;
				break;
			case '[':
				if (multi_char_entry) {
					parsed_expression[current_index] += part;
				} else {
					parsed_expression.push_back(part);
				}

				if (!escaped) {
					square_brackets_open = true;
				}

				escaped = false;
				caret = false;
				break;

			case ']':
				if (multi_char_entry) {
					parsed_expression[current_index] += part;
				} else {
					parsed_expression.push_back(part);
				}

				if (!escaped) {
					square_brackets_open = false;
				}

				escaped = false;
				caret = false;
				break;
			
			case '(':
				if (!escaped) {
					parentheses_level++;
				}

				if (multi_char_entry) {
					parsed_expression[current_index] += part;
				} else {
					parsed_expression.push_back(part);
				}

				escaped = false;
				caret = false;
				break;
			
			case ')':
				if (!escaped) {
					parentheses_level--;
				}

				if (multi_char_entry) {
					parsed_expression[current_index] += part;
				} else {
					parsed_expression.push_back(part);
				}

				escaped = false;
				caret = false;
				break;
			
			case '{':
				if (!escaped && !square_brackets_open) {
					curly_braces_open = true;
					curly_braces_buffer += part;
				} else {
					if (multi_char_entry) {
						parsed_expression[current_index] += part;
					} else {
						parsed_expression.push_back(part);
					}
				}

				escaped = false;
				caret = false;
				break;
			
			case '}':
				if (curly_braces_open && curly_braces_received_number) {
					curly_braces_open = false;
					curly_braces_received_number = false;
					curly_braces_received_comma = false;
					curly_braces_buffer += part;

					parsed_expression[current_index] += curly_braces_buffer;

					curly_braces_buffer = "";
				} else {
					if (multi_char_entry) {
						parsed_expression[current_index] += part;
					} else {
						parsed_expression.push_back(part);
					}
				}

				escaped = false;
				caret = false;
				break;

			case '+':
				parsed_expression[current_index] += part;

				escaped = false;
				caret = false;
				break;
			
			case '*':
				parsed_expression[current_index] += part;

				escaped = false;
				caret = false;
				break;
			
			case '?':
				parsed_expression[current_index] += part;

				escaped = false;
				caret = false;
				break;
			
			case '^':
				if (!escaped) {
					caret = true;
				} 
				
				if (multi_char_entry) {
					parsed_expression[current_index] += part;
				} else {
					parsed_expression.push_back(part);
				}

				escaped = false;
				break;
			
			case '$':
				parsed_expression[current_index] += part;

				escaped = false;
				caret = false;
				break;

			default:
				if (curly_braces_open) {
					// The curly braces ("{}") only have special meaning if they're followed by an INTEGER
					// Ie, [a-z]{2} or [a-z]{2,3} or [a-z]{2,}
					// Does not count: [a-z]{two} (this expression would look for '{' 't' 'w' 'o' '}' literal)
					// Does not count: [a-z]{,}

					if (
						(!is_number(part) && part != ",")
						|| (part == "," && (!curly_braces_received_number || curly_braces_received_comma))
					) {
						// If we're getting something other than a number,
						// (or ONE SINGLE comma AFTER we've already received a number)
						// Give up on the curly_braces_buffer
						curly_braces_open = false;
						curly_braces_received_number = false;

						// Push each of the characters we had been saving for the buffer
						// Back into the output vector one by one
						for (int i=0; i<curly_braces_buffer.size(); i++) {
							std::string bufferpart(1, curly_braces_buffer[i]);

							if (multi_char_entry) {
								parsed_expression[current_index] += bufferpart;
							} else {
								parsed_expression.push_back(bufferpart);
							}
						}

						// Clear the buffer
						curly_braces_buffer = "";
						if (multi_char_entry) {
							parsed_expression[current_index] += part;
						} else {
							parsed_expression.push_back(part);
						}
					} else {
						curly_braces_received_number = true;
						if (part == ",") {
							curly_braces_received_comma = true;
						}
						curly_braces_buffer += part;
					}
				} else {
					if (multi_char_entry) {
						parsed_expression[current_index] += part;
					} else {
						parsed_expression.push_back(part);
					}
				}

				escaped = false;
				caret = false;
		}
	}

	return parsed_expression;
}