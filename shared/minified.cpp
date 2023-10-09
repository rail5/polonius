#ifndef STDIOH
	#define STDIOH
	#include <stdio.h>
#endif

#ifndef INTTYPESH
	#define INTTYPESH
	#include <inttypes.h>
#endif

#ifndef STRINGH
	#define STRINGH
	#include <string.h>
#endif

#ifndef STDLIBH
	#define STDLIBH
	#include <stdlib.h>
#endif

#define correct_output_stream ((type() == standard) ? stdout : stderr)

namespace minified {

	/***
	 * STRINGS
	 * These strings support only basic functionality:
		 * .length()
		 * 
		 * .c_str()
		 * 
		 * assignment operator '='
		 * 	e.g:
		 * 	minified::string text = "abc";
		 * 
		 * comparison operators '==' and '!='
		 * 
		 * concatenation operators '+' and '+='
		 * 	e.g:
		 * 	minified::string text = "ab" + "c";
		 * 	text += "123";
		 * 
		 * simple constructors
		 * 	e.g:
		 * 	minified::string text("abc");
		 * 	
		 * 	minified::string text(length, "text");
		 * 
		 * array index operator '[]'
		 * 	e.g:
		 * 	printf("%c", string[3]);
	***/

	class string {
		protected:
			char* string_contents;
			int64_t string_length = 0;
		
		public:
			string& operator=(const char* text) {
				string_length = strlen(text);
				string_contents = (char*) malloc(string_length + 1);
				strncpy(string_contents, text, string_length + 1);
				return *this;
			}

			string& operator+=(const char* text) {
				string_length = string_length + strlen(text);
				string_contents = (char*) realloc(string_contents, string_length + 1);
				strncat(string_contents, text, string_length + 1);
				return *this;
			}

			friend string operator+(const string& text_one, const string& text_two) {
				int64_t length = (text_one.string_length + text_two.string_length);
				char* contents = new char[length + 1];

				strncpy(contents, text_one.string_contents, length + 1);
				strncat(contents, text_two.string_contents, length + 1);

				string result(contents);

				delete[] contents;

				return result;
			}

			friend bool operator==(const string& text_one, const string& text_two) {
				return (strcmp(text_one.string_contents, text_two.string_contents) == 0);
			}

			friend bool operator!=(const string& text_one, const string& text_two) {
				return (strcmp(text_one.string_contents, text_two.string_contents) != 0);
			}

			char& operator[](int64_t index) {
				return this->c_str()[index];
			}

			string(const char* text = "") {
				string_length = strlen(text);
				string_contents = (char*) malloc(string_length + 1);
				strncpy(string_contents, text, string_length + 1);
			}

			string(int64_t size, const char* text = "") {
				string_length = size;
				string_contents = (char*) malloc(string_length + 1);
				strncpy(string_contents, text, string_length + 1);
			}

			~string() {
				free(string_contents);
			}

			char* c_str() {
				return string_contents;
			}

			int64_t length() {
				return string_length;
			}

			string substr(int64_t start, int64_t length) {
				// How?
			}
	};


	/***
	 * OUTPUT STREAMS
	 * Minimal imitations of 'cout' and 'cerr'
	***/

	enum output_stream_type { standard, error };

	class output_stream {
		protected:
			virtual output_stream_type type() {
				return standard;
			}

			void output(string content) {
				fprintf(correct_output_stream, content.c_str());
			}

			void output(char content) {
				fprintf(correct_output_stream, "%c", content);
			}

			void output(int content) {
				fprintf(correct_output_stream, "%i", content);
			}

			void output(int64_t content) {
				fprintf(correct_output_stream, "%" PRId64, content);
			}

			void output(uint64_t content) {
				fprintf(correct_output_stream, "%" PRIu64, content);
			}

			void output(double content) {
				fprintf(correct_output_stream, "%d", content);
			}
		
		public:
			output_stream& operator<<(string content) {
				output(content.c_str());
				return *this;
			}

			output_stream& operator<<(char content) {
				output(content);
				return *this;
			}

			output_stream& operator<<(int content) {
				output(content);
				return *this;
			}

			output_stream& operator<<(int64_t content) {
				output(content);
				return *this;
			}

			output_stream& operator<<(uint64_t content) {
				output(content);
				return *this;
			}

			output_stream& operator<<(double content) {
				output(content);
				return *this;
			}
	};

	class cout_stream : public output_stream {
		output_stream_type type() {
			return standard;
		}
	};

	class cerr_stream : public output_stream {
		output_stream_type type() {
			return error;
		}
	};

	const char* endl = "\n";

	cout_stream cout;
	cerr_stream cerr;

}