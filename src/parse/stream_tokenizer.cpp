#include "parse/stream_tokenizer.h"

#include <algorithm>

namespace simpledb {
void StreamTokenizer::ResetSyntax() noexcept {
  for (int i = ctype_.size(); --i >= 0;) {
    ctype_[i] = 0;
  }
}

void StreamTokenizer::WordChars(int low, int hi) noexcept {
  if (low < 0) {
    low = 0;
  }
  int size = ctype_.size();
  if (hi >= size) {
    hi = size - 1;
  }
  while (low <= hi) {
    ctype_[low++] |= CT_ALPHA;
  }
}

void StreamTokenizer::WhitespaceChars(int low, int hi) noexcept {
  if (low < 0) {
    low = 0;
  }
  int size = ctype_.size();
  if (hi >= size) {
    hi = size - 1;
  }
  while (low <= hi) {
    ctype_[low++] = CT_WHITESPACE;
  }
}

void StreamTokenizer::OrdinaryChars(int low, int hi) noexcept {
  if (low < 0) {
    low = 0;
  }
  int size = ctype_.size();
  if (hi >= size) {
    hi = size - 1;
  }
  while (low <= hi) {
    ctype_[low++] = 0;
  }
}

void StreamTokenizer::OrdinaryChar(int ch) noexcept {
  if (ch >= 0 && ch < static_cast<int>(ctype_.size())) {
    ctype_[ch] = 0;
  }
}

void StreamTokenizer::CommentChar(int ch) noexcept {
  if (ch >= 0 && ch < static_cast<int>(ctype_.size())) {
    ctype_[ch] = CT_COMMENT;
  }
}

void StreamTokenizer::QuoteChar(int ch) noexcept {
  if (ch >= 0 && ch < static_cast<int>(ctype_.size())) {
    ctype_[ch] = CT_QUOTE;
  }
}

void StreamTokenizer::ParseNumbers() noexcept {
  for (int i = '0'; i <= '9'; i++) {
    ctype_[i] |= CT_DIGIT;
  }
  ctype_['.'] |= CT_DIGIT;
  ctype_['-'] |= CT_DIGIT;
}

int StreamTokenizer::NextToken() {
  if (pushed_back_) {
    pushed_back_ = false;
    return ttype;
  }

  sval = "";
  int eof = std::istringstream::traits_type::eof();

  int c = peekc_;
  if (c == eof) {
    c = NEED_CHAR;
  }
  if (c == SKIP_LF) {
    c = input_.get();
    if (c == eof) {
      return ttype = TT_EOF;
    }
    if (c == '\n') {
      c = NEED_CHAR;
    }
  }
  if (c == NEED_CHAR) {
    c = input_.get();
    if (c == eof) {
      return ttype = TT_EOF;
    }
  }
  ttype = c; /* Just to be safe */

  /**
   * Set `peekc_` so that the next invocation of `NextToken` will read another
   * character unless `peekc_` is reset in this invocation
   */
  peekc_ = NEED_CHAR;

  int ctype = (c < 256) ? ctype_[c] : CT_ALPHA;
  while ((ctype & CT_WHITESPACE) != 0) {
    if (c == '\r') {
      LINENO++;
      if (eol_is_significant_p_) {
        peekc_ = SKIP_LF;
        return ttype = TT_EOL;
      }
      c = input_.get();
      if (c == '\n') {
        c = input_.get();
      }
    } else {
      if (c == '\n') {
        LINENO++;
        if (eol_is_significant_p_) {
          return ttype = TT_EOL;
        }
      }
      c = input_.get();
    }
    if (c == eof) {
      return ttype = TT_EOF;
    }
    ctype = (c < 256) ? ctype_[c] : CT_ALPHA;
  }

  if ((ctype & CT_DIGIT) != 0) {
    bool neg = false;
    if (c == '-') {
      c = input_.get();
      if (c != '.' && (c < '0' || c > '9')) {
        peekc_ = c;
        return ttype = '-';
      }
      neg = true;
    }
    double v = 0;
    int dec_exp = 0;
    int seen_dot = 0;
    while (true) {
      if (c == '.' && seen_dot == 0) {
        seen_dot = 1;
      } else if ('0' <= c && c <= '9') {
        v = v * 10 + (c - '0');
        dec_exp += seen_dot;
      } else {
        break;
      }
      c = input_.get();
    }
    peekc_ = c;
    if (dec_exp != 0) {
      double denom = 10;
      dec_exp--;
      while (dec_exp > 0) {
        denom *= 10;
        dec_exp--;
      }
      /* Do one division of a likely-to-be-more-accurate number */
      v = v / denom;
    }
    nval = neg ? -v : v;
    return ttype = TT_NUMBER;
  }

  if ((ctype & CT_ALPHA) != 0) {
    buf_.clear();
    do {
      buf_.push_back(static_cast<char>(c));
      c = input_.get();
      ctype = (c == eof) ? CT_WHITESPACE : ((c < 256) ? ctype_[c] : CT_ALPHA);
    } while ((ctype & (CT_ALPHA | CT_DIGIT)) != 0);
    peekc_ = c;
    sval = std::string{buf_.begin(), buf_.end()};
    if (force_lower_) {
      std::transform(sval.begin(), sval.end(), sval.begin(),
                     [](unsigned char c) { return std::tolower(c); });
    }
    return ttype = TT_WORD;
  }

  if ((ctype & CT_QUOTE) != 0) {
    ttype = c;
    buf_.clear();
    /* Invariants (because \Octal needs a lookahead):
     *   (i)  c contains char value
     *   (ii) d contains the lookahead
     */
    int d = input_.get();
    while (d != eof && d != ttype && d != '\n' && d != '\r') {
      if (d == '\\') {
        c = input_.get();
        int first = c; /* To allow \377, but not \477 */
        if (c >= '0' && c <= '7') {
          c = c - '0';
          int c2 = input_.get();
          if ('0' <= c2 && c2 <= '7') {
            c = (c << 3) + (c2 - '0');
            c2 = input_.get();
            if ('0' <= c2 && c2 <= '7' && first <= '3') {
              c = (c << 3) + (c2 - '0');
              d = input_.get();
            } else {
              d = c2;
            }
          } else {
            d = c2;
          }
        } else {
          switch (c) {
            case 'a':
              c = 0x7;
              break;
            case 'b':
              c = '\b';
              break;
            case 'f':
              c = 0xC;
              break;
            case 'n':
              c = '\n';
              break;
            case 'r':
              c = '\r';
              break;
            case 't':
              c = '\t';
              break;
            case 'v':
              c = 0xB;
              break;
          }
          d = input_.get();
        }
      } else {
        c = d;
        d = input_.get();
      }
      buf_.push_back(static_cast<char>(c));
    }

    /* If we broke out of the loop because we found a matching quote
     * character then arrange to read a new character next time
     * around; otherwise, save the character.
     */
    peekc_ = (d == ttype) ? NEED_CHAR : d;

    sval = std::string{sval.begin(), sval.end()};
    return ttype;
  }

  if (c == '/' && (slash_slash_comments_p_ || slash_star_comments_p_)) {
    c = input_.get();
    if (c == '*' && slash_star_comments_p_) {
      int prevc = 0;
      while ((c = input_.get()) != '/' || prevc != '*') {
        if (c == '\r') {
          LINENO++;
          c = input_.get();
          if (c == '\n') {
            c = input_.get();
          }
        } else {
          if (c == '\n') {
            LINENO++;
            c = input_.get();
          }
        }
        if (c < 0) {
          return ttype = TT_EOF;
        }
        prevc = c;
      }
      return NextToken();
    } else if (c == '/' && slash_slash_comments_p_) {
      while ((c = input_.get()) != '\n' && c != '\r' && c != eof) {
      }
      peekc_ = c;
      return NextToken();
    } else {
      /* Now see if it is still a single line comment */
      if ((ctype_['/'] & CT_COMMENT) != 0) {
        while ((c = input_.get()) != '\n' && c != '\r' && c != eof) {
        }
        peekc_ = c;
        return NextToken();
      } else {
        peekc_ = c;
        return ttype = '/';
      }
    }
  }

  if ((ctype & CT_COMMENT) != 0) {
    while ((c = input_.get()) != '\n' && c != '\r' && c != eof) {
    }
    peekc_ = c;
    return NextToken();
  }

  return ttype = c;
}

void StreamTokenizer::PushBack() noexcept {
  if (ttype != TT_NOTHING) { /* No-op if `NextToken` not called */
    pushed_back_ = true;
  }
}

std::string StreamTokenizer::ToString() const {
  std::string result;
  switch (ttype) {
    case TT_EOF:
      result = "EOF";
      break;
    case TT_EOL:
      result = "EOL";
      break;
    case TT_WORD:
      result = sval;
      break;
    case TT_NUMBER:
      result = "n=" + std::to_string(nval);
      break;
    case TT_NOTHING:
      result = "nothing";
      break;
    default:
      if (ttype < 256 && ((ctype_[ttype] & CT_QUOTE) != 0)) {
        result = sval;
      }
      result = std::string{3, '\0'};
      result[0] = result[2] = '\'';
      result[1] = static_cast<char>(ttype);
      break;
  }

  return "Token[" + result + "], line " + std::to_string(LINENO);
}
}  // namespace simpledb
