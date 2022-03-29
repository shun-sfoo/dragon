#ifndef SOUNDEX_H
#define SOUNDEX_H

#include <cctype>
#include <string>
#include <unordered_map>
using namespace std;

static const size_t MaxCodeLength{4};
const string NotADigit{"*"};

class Soundex {
 public:
  string encode(const string& word) const {
    return zeroPad(upperFront(head(word)) + tail(encodedDigits(word)));
  }

  string encodedDigit(char letter) const {
    const unordered_map<char, string> encodings{
        {'b', "1"}, {'f', "1"}, {'p', "1"}, {'v', "1"},

        {'c', "2"}, {'g', "2"}, {'j', "2"}, {'k', "2"},
        {'q', "2"}, {'x', "2"}, {'s', "2"}, {'z', "2"},

        {'d', "3"}, {'t', "3"},

        {'l', "4"},

        {'m', "5"}, {'n', "5"},

        {'r', "6"}

    };

    auto it = encodings.find(lower(letter));
    return it == encodings.end() ? NotADigit : it->second;
  }

 private:
  string head(const string& word) const {
    auto encoded = word.substr(0, 1);
    return encoded;
  }

  string tail(const string& word) const { return word.substr(1); }

  bool isComplete(const string& word) const {
    return word.length() == MaxCodeLength;
  }

  string lastDigit(const string& encoding) const {
    if (encoding.empty())
      return NotADigit;
    return string(1, encoding.back());
  }

  string upperFront(const string& word) const {
    return string(1, toupper(static_cast<unsigned char>(word.front())));
  }

  char lower(char& c) const { return tolower(static_cast<unsigned char>(c)); }

  string encodedDigits(const string& word) const {
    string encoding;
    encodeHead(encoding, word);
    encodeTail(encoding, word);
    return encoding;
  }

  void encodeHead(string& encoding, const string& word) const {
    encoding += encodedDigit(word.front());
  }

  void encodeTail(string& encoding, const string& word) const {
    for (auto i = 1u; i < word.length(); i++) {
      if (!isComplete(encoding))
        encodeLetter(encoding, word[i], word[i - 1]);
    }
  }

  void encodeLetter(string& encoding, char letter, char lastLetter) const {
    auto digit = encodedDigit(letter);
    if (digit != NotADigit &&
        (digit != lastDigit(encoding) || isVowel(lastLetter)))
      encoding += digit;
  }

  bool isVowel(char letter) const {
    return string("aeiouy").find(lower(letter)) != string::npos;
  }

  string zeroPad(const string& word) const {
    auto zerosNeeded = MaxCodeLength - word.length();
    return word + string(zerosNeeded, '0');
  }
};

#endif
