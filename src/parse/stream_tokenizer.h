#pragma once

#include <array>
#include <sstream>
#include <string>
#include <vector>

namespace simpledb {
/**
 * The `StreamTokenizer` class takes an input string and parses it into
 * "tokens", allowing the tokens to be read one at a time. The parsing process
 * is controlled by a table and a number of flags that can be set to various
 * states. The stream tokenizer can recognize identifiers, numbers, quoted
 * strings, and various comment styles.
 *
 * Each byte read from the input stream is regarded as a character in the range
 * '\u0000' through '\u00FF'. The character value is used to look up five
 * possible attributes of the character: white space, alphabetic, numeric,
 * string quote, and comment character. Each character can have zero or more of
 * these attributes.
 *
 * In addition, an instance has four flags. These flags indicate:
 * - Whether line terminators are to be returned as tokens or treated as white
 *   space that merely separates tokens.
 * - Whether C-style comments are to be recognized and skipped.
 * - Whether C++-style comments are to be recognized and skipped.
 * - Whether the characters of identifiers are converted to lowercase.
 *
 * A typical application first constructs an instance of this class, sets up the
 * syntax tables, and then repeatedly loops calling the `NextToken` method in
 * each iteration of the loop until it returns the value `TT_EOF`.
 *
 * The original implementation of this class is by James Gosling in Java at
 * https://github.com/openjdk/jdk/blob/master/src/java.base/share/classes/java/io/StreamTokenizer.java
 */
class StreamTokenizer {
 public:
  /**
   * @brief Create a stream tokenizer that parses the specified input string.
   * The stream tokenizer is initialized to the following default state:
   * - All byte values 'A' through 'Z', 'a' through 'z', and '\u00A0' through
   *   '\u00FF' are considered to be alphabetic.
   * - All byte values '\u0000' through '\u0020' are considered to be white
   *   space.
   * - '/' is a comment character.
   * - Single quote '\'' and double quote '"' are string quote characters.
   * - Numbers are parsed.
   * - Ends of lines are treated as white space, not as separate tokens.
   * - C-style and C++-style comments are not recognized.
   *
   * We use universal reference as parameter type instead of string_view since
   * `istringstream` constructor does not accept string_view as argument The.
   * However, this is possible from C++26. The requires-clause prevents this
   * custom constructor from shadowing the copy constructor
   *
   * @param is an input string
   */
  template <typename Str>
    requires(!std::is_same_v<std::decay_t<Str>, StreamTokenizer>)
  explicit StreamTokenizer(Str&& is) : input_(std::forward<Str>(is)), buf_(20) {
    WordChars('a', 'z');
    WordChars('A', 'Z');
    WordChars(128 + 32, 255);
    WhitespaceChars(0, ' ');
    CommentChar('/');
    QuoteChar('"');
    QuoteChar('\'');
    ParseNumbers();
  }

  /**
   * @brief Reset this tokenizer's syntax table so that all characters are
   * "ordinary."
   */
  void ResetSyntax() noexcept;

  /**
   * @brief Specify that all characters `c` in the range `low <= c <= high`
   * are word constituents. A word token consists of a word constituent followed
   * by zero or more word constituents or number constituents.
   *
   * @param low the low end of the range
   * @param hi the high end of the range
   */
  void WordChars(int low, int hi) noexcept;

  /**
   * @brief Specify that all characters `c` in the range `low <= c <= high` are
   * white space characters. White space characters serve only to separate
   * tokens in the input stream.
   *
   * Any other attribute settings for the characters in the specified range are
   * cleared.
   *
   * @param low the low end of the range
   * @param hi the high end of the range
   */
  void WhitespaceChars(int low, int hi) noexcept;

  /**
   * @brief Specify that all characters `c` in the range `low <= c <= high` are
   * "ordinary" in this tokenizer.
   *
   * @param low the low end of the range
   * @param hi the high end of the range
   */
  void OrdinaryChars(int low, int hi) noexcept;

  /**
   * @brief Specify that the character argument is "ordinary" in this tokenizer.
   * It removes any special significance the character has as a comment
   * character, word component, string delimiter, white space, or number
   * character. When such a character is encountered by the parser, the parser
   * treats it as a single-character token and sets `ttype` field to the
   * character value.
   *
   * Making a line terminator character "ordinary" may interfere with the
   * ability of a `StreamTokenizer` to count lines. The `Lineno` method may no
   * longer reflect the presence of such terminator characters in its line
   * count.
   *
   * @param ch the character
   */
  void OrdinaryChar(int ch) noexcept;

  /**
   * @brief Specify that the character argument starts a single-line comment.
   * All characters from the comment character to the end of the line are
   * ignored by this stream tokenizer.
   *
   * Any other attribute settings for the specified character are cleared.
   *
   * @param ch the character
   */
  void CommentChar(int ch) noexcept;

  /**
   * @brief Specify that the matching pairs of this character delimit string
   * constants in this tokenizer.
   *
   * When the `NextToken` method encounters a string constant, the `ttype` field
   * is set to the string delimiter and the `sval` field is set to the body of
   * the string.
   *
   * If a string quote character is encountered, then a string is recognized,
   * consisting of all characters after (but not including) the string quote
   * character, up to (but not including) the next occurrence of that same
   * string quote character, or a line terminator, or end of file. The usual
   * escape sequences such as "\n" and "\t" are recognized and converted to
   * single characters as the string is parsed.
   *
   * Any other attribute settings for the specified character are cleared.
   *
   * @param ch the character
   */
  void QuoteChar(int ch) noexcept;

  /**
   * @brief Specify that numbers should be parsed by this tokenizer. The syntax
   * table of this tokenizer is modified so that each of the twelve characters:
   *   0 1 2 3 4 5 6 7 8 9 . -
   * has the "numeric" attribute.
   *
   * When the parser encounters a word token that has the format of a double
   * precision floating-point number, it treats the token as a number rather
   * than a word, by setting the `ttype` field to the value `TT_NUMBER` and
   * putting the numeric value of the token into the `nval` field.
   */
  void ParseNumbers() noexcept;

  /**
   * @brief Determine whether or not ends of line are treated as tokens. If the
   * flag argument is true, this tokenizer treats ends of lines as tokens; the
   * `NextToken` method returns `TT_EOL` and also sets the `ttype` field to this
   * value when an end of line is read.
   *
   * A line is a sequence of characters ending with either a carriage-return
   * character '\r' or a newline character '\n'. In addition, a carriage-return
   * character followed immediately by a newline character is treated as a
   * single end-of-line token.
   *
   * If the `flag` is false, end-of-line characters are treated as white space
   * and serve only to separate tokens.
   *
   * @param flag `true` indicates that end-of-line characters are separate
   * tokens; `false` indicates that end-of-line characters are white space.
   */
  void EOLIsSignificant(bool flag) noexcept { eol_is_significant_p_ = flag; }

  /**
   * @brief Determine whether or not the tokenizer recognizes C-style comments.
   * If the flag argument is `true`, this stream tokenizer recognizes C-style
   * comments. All text between successive occurrences of / * and * / are
   * discarded.
   *
   * If the flag argument is `false`, then C-style comments are not treated
   * specially.
   *
   * @param flag `true` indicates to recognize and ignore C-style comments.
   */
  void SlashStarComments(bool flag) noexcept { slash_star_comments_p_ = flag; }

  /**
   * @brief Determine whether or not the tokenizer recognizes C++-style
   * comments. If the flag argument is `true`, this stream tokenizer recognizes
   * C++-style comments. Any occurrence of two consecutive slash characters '/'
   * is treated as the beginning of a comment that extends to the end of the
   * line.
   *
   * If the flag argument is `false`, then C++-style comments are not treated
   * specially.
   *
   * @param flag `true` indicates to recognize and ignore C++-style comments.
   */
  void SlashSlashComments(bool flag) noexcept {
    slash_slash_comments_p_ = flag;
  }

  /**
   * @brief Determine whether or not word tokens are automatically lowercased.
   * If the flag argument is `true`, then the value in the `sval` field is
   * lowercased whenever a word token is returned (the `ttype` field has the
   * value `TT_WORD`) by the `NextToken` method of this tokenizer.
   *
   * If the flag argument is `false`, then the `sval` field is not modified.
   *
   * @param flag `true` indicates that all word tokens should be lowercased.
   */
  void LowerCaseMode(bool flag) noexcept { force_lower_ = flag; }

  /**
   * @brief Parse the next token from the input stream of this tokenizer. The
   * type of the next token is returned in the `ttype` field. Additional
   * information about the token may be in the `nval` field or the `sval` field
   * of this tokenizer.
   *
   * Typical clients of this class first set up the syntax tables and then sit
   * in a loop calling `NextToken` to parse successive tokens until `TT_EOF` is
   * returned.
   *
   * @return the value of the `ttype` field
   */
  int NextToken();

  /**
   * @brief Cause the next call to the `NextToken` method of this tokenizer to
   * return the current value in the `ttype` field, and not to modify the value
   * in the `nval` or `sval` field.
   */
  void PushBack() noexcept;

  /**
   * @brief Return the current line number.
   * @return the current line number of this stream tokenizer.
   */
  int Lineno() const noexcept { return LINENO; }

  /**
   * @brief Return the string representation of the current stream token and the
   * line number it occurs on.
   *
   * @return a string representation of the token
   */
  std::string ToString() const;

 private:
  std::istringstream input_;
  std::vector<char> buf_;

  /**
   * The next character to be considered by the `NextToken` method. May
   * also be `NEED_CHAR` to indicate that a new character should be
   * read, or `SKIP_LF` to indicate that a new character should be read
   * and, if it is a '\n' character, it should be discarded and a second
   * new character should be read.
   */
  int peekc_{NEED_CHAR};

  static constexpr int NEED_CHAR{INT_MAX};
  static constexpr int SKIP_LF{INT_MAX - 1};

  bool pushed_back_{false};
  bool force_lower_{false};
  /** The line number of the last token read */
  int LINENO{1};

  bool eol_is_significant_p_{false};
  bool slash_slash_comments_p_{false};
  bool slash_star_comments_p_{false};

  std::array<int8_t, 256> ctype_{};
  static constexpr int8_t CT_WHITESPACE{1};
  static constexpr int8_t CT_DIGIT{2};
  static constexpr int8_t CT_ALPHA{4};
  static constexpr int8_t CT_QUOTE{8};
  static constexpr int8_t CT_COMMENT{16};

 public:
  /**
   * After a call to the `NextToken` method, this field contains the
   * type of the token just read. For a single character token, its
   * value is the single character, converted to an integer. For a
   * quoted string token, its value is the quote character. Otherwise,
   * its value is one of the following:
   * - `TT_WORD` indicates that the token is a word.
   * - `TT_NUMBER` indicates that the token is a number.
   * - `TT_EOL` indicates that the end of line has been read.
   *   This field can only have this value if the `EOLIsSignificant`
   *   method has been called with the argument `true`.
   * - `TT_EOF` indicates that the end of the input stream has been
   * reached.
   *
   * The initial value of this field is `TT_NOTHING`.
   */
  int ttype{TT_NOTHING};

  /**
   * A constant indicating that the end of the stream has been read.
   */
  static constexpr int TT_EOF{-1};

  /**
   * A constant indicating that the end of the line has been read.
   */
  static constexpr int TT_EOL{'\n'};

  /**
   * A constant indicating a number token has been read.
   */
  static constexpr int TT_NUMBER{-2};

  /**
   * A constant indicating that a word token has been read.
   */
  static constexpr int TT_WORD{-3};

  /**
   * A constant indicating that no token has been read, used for
   * initializing `ttype`.
   */
  static constexpr int TT_NOTHING{-4};

  /**
   * If the current token is a word token, this field contains a string
   * giving the characters of the word token. When the current token is
   * a quoted string token, this field contains the body of the string.
   *
   * The current token is a word when the value of the `ttype` field is
   * `TT_WORD`. The current token is a quoted string token when the
   * value of the `ttype` field is a quote character.
   */
  std::string sval;

  /**
   * If the current token is a number, this field contains the value of
   * that number. The current token is a number when the value of the
   * `ttype` field is `TT_NUMBER`.
   *
   * The initial value of this field is 0.0.
   */
  double nval{};
};
}  // namespace simpledb
