#include "universaljapanesekanalookup.hpp"

#include <vector>
#include <map>

/// NOTE:
///  This classes are part of another project of mine, since they fit
///  into this project, I copied them over.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** CLASS: KanaFix
 * This class fixes kana characters made with the unicode characters
 *   [KANA] + [STANDALONE DAKUTEN(濁点)]
 * with its single kana character which already includes the [DAKUTEN(濁点)].
 * Source of such (broken?) characters are usually iOS/Android and some Japanese IMEs for Windows.
 *
 * My primitive search mechanism doesn't like this and skips actual results :/
 */
class KanaFix
{
public:
  enum class Mode {
      Default,
      Abnormal
  };

  // Parses the whole string and fixes all kana characters with (broken?) dakuten.
  void fixChar(QString &str) const
  {
      // analyze string for kanas
      for (int i = 0; i < str.size(); i++)
      {
          if (this->isKana(str.at(i).unicode()))
          {
              // check for dakuten after kana
              if ( (i+1) > str.size() ) break;
              if (this->isDakuten(str.at(i+1).unicode()))
              {
                  /// replace: kana+dakuten with dakuten included kana
                  str.replace(i, 2, this->getKanaWDakuten(str.at(i).unicode(), str.at(i+1).unicode()));
              } // endOf: isDakuten
          } // endOf: isKana
      }
  }

  // Parses the whole string and seperates all kana characters to its kana and dakuten.
  void breakChar(QString &str, Mode mode = Mode::Default) const
  {
      // analyze string for kanas
      for (int i = 0; i < str.size(); i++)
      {
          if (this->isKanaWDakuten(str.at(i).unicode()))
          {
              std::vector<QChar> data = this->getKanaDakutenSep(str.at(i).unicode(), mode);
              str.replace(i, data.at(0));
              str.insert(i+1, data.at(1));
              i++;
          } // endOf: isKana
      }
  }

private:
  enum class Dakuten {
    // JIS X 0216
    VOICED_SOUND_MARK,   // "
    VOICED_SOUND_MARK_1, //
    VOICED_SOUND_MARK_2, //
    SEMI_VOICED_SOUND    // °
  };

  bool isKana(ushort c) const
  {
      for (auto it = this->kana_map.begin(); it != this->kana_map.end(); it++)
          if (c == it->unicode())
              return true;
      return false;
  }

  bool isKanaWDakuten(ushort c) const
  {
      for (auto it = this->kana_dakuten_map.begin(); it != this->kana_dakuten_map.end(); it++)
          if (c == it->unicode())
              return true;
      return false;
  }

  bool isDakuten(ushort c) const
  {
      for (auto it = this->dakuten_map.begin(); it != this->dakuten_map.end(); it++)
          if (c == it->second.unicode())
              return true;
      return false;
  }

  QChar getKanaWDakuten(ushort kana, ushort dakuten) const
  {
    #define check_dakuten(type) dakuten == this->dakuten_map.at(Dakuten::type)

    // search kana pos in kana_map
    int pos = 0;
    for (; pos < (int) this->kana_map.size(); pos++)
        if (kana == this->kana_map.at(pos).unicode())
            break;

    // check type of dakuten(濁点)
    if (check_dakuten(SEMI_VOICED_SOUND))           // °
        return this->kana_dakuten_map.at(pos + 10);

    else if (check_dakuten(VOICED_SOUND_MARK)   ||  // "
             check_dakuten(VOICED_SOUND_MARK_1) ||
             check_dakuten(VOICED_SOUND_MARK_2))
        return this->kana_dakuten_map.at(pos);

    return QChar();
    #undef check_dakuten
  }

  std::vector<QChar> getKanaDakutenSep(ushort kana, Mode mode) const
  {
      // search kana pos in kana_dakuten_map
      int pos = 0;
      for(; pos < (int) this->kana_dakuten_map.size(); pos++)
          if (kana == this->kana_dakuten_map.at(pos).unicode())
              break;
      bool isSemi = false;
      if (pos > (int) this->kana_map.size()) { pos-= 10; isSemi = true; }

      std::vector<QChar> data; data.clear(); data.resize(2);
      switch (mode)
      {
          case Mode::Default:
              data[0] = this->kana_map.at(pos);
              if (isSemi) data[1] = this->dakuten_map.at(Dakuten::SEMI_VOICED_SOUND);
              else        data[1] = this->dakuten_map.at(Dakuten::VOICED_SOUND_MARK);
              //return QString(data, 2);
              return data;
              break;

          case Mode::Abnormal:
              data[0] = this->kana_map.at(pos);
              if (isSemi) data[1] = this->dakuten_map.at(Dakuten::SEMI_VOICED_SOUND);
              else        data[1] = this->dakuten_map.at(Dakuten::VOICED_SOUND_MARK_1);
              //return QString(data, 2);
              return data;
              break;
      }

      //return QString();
      return data;
  }

  // Contains all kanas that can have a dakuten (濁点).
  static const std::vector<QChar> kana_map;

  // Contains all kanas with dakuten (濁点) included.
  static const std::vector<QChar> kana_dakuten_map;

  // Contains all kinds on dakuten, actually there are only 2 of it ;-)
  static const std::map<Dakuten, QChar> dakuten_map;
};

#define KanaMap(id_h, id_k) QChar(id_h), QChar(id_k)
const std::vector<QChar> KanaFix::kana_map = {
    KanaMap(0x304B, 0x30AB), // か, カ
    KanaMap(0x304D, 0x30AD), // き, キ
    KanaMap(0x304F, 0x30AF), // く, ク
    KanaMap(0x3051, 0x30B1), // け, ヶ
    KanaMap(0x3053, 0x30B3), // こ, コ

    KanaMap(0x3055, 0x30B5), // さ, サ
    KanaMap(0x3057, 0x30B7), // し, シ
    KanaMap(0x3059, 0x30B9), // す, ス
    KanaMap(0x305B, 0x30BB), // せ, セ
    KanaMap(0x305D, 0x30BD), // そ, ソ

    KanaMap(0x305F, 0x30BF), // た, タ
    KanaMap(0x3061, 0x30C1), // ち, チ
    KanaMap(0x3064, 0x30C4), // つ, ツ
    KanaMap(0x3066, 0x30C6), // て, テ
    KanaMap(0x3068, 0x30C8), // と, ト

    KanaMap(0x306F, 0x30CF), // は, ハ
    KanaMap(0x3072, 0x30D2), // ひ, ヒ
    KanaMap(0x3075, 0x30D5), // ふ, フ
    KanaMap(0x3078, 0x30D8), // へ, ヘ
    KanaMap(0x307B, 0x30DB), // ほ, ホ
};

const std::vector<QChar> KanaFix::kana_dakuten_map = {
    KanaMap(0x304C, 0x30AC), // が, ガ
    KanaMap(0x304E, 0x30AE), // ぎ, ギ
    KanaMap(0x3050, 0x30B0), // ぐ, グ
    KanaMap(0x3052, 0x30B2), // げ, ゲ
    KanaMap(0x3054, 0x30B4), // ご, ゴ

    KanaMap(0x3056, 0x30B6), // ざ, ザ
    KanaMap(0x3058, 0x30B8), // じ, ジ
    KanaMap(0x305A, 0x30BA), // ず, ズ
    KanaMap(0x305C, 0x30BC), // ぜ, ゼ
    KanaMap(0x305E, 0x30BE), // ぞ, ゾ

    KanaMap(0x3060, 0x30C0), // だ, ダ
    KanaMap(0x3062, 0x30C2), // ぢ, ヂ
    KanaMap(0x3065, 0x30C5), // づ, ヅ
    KanaMap(0x3067, 0x30C7), // で, デ
    KanaMap(0x3069, 0x30C9), // ど, ド

    KanaMap(0x3070, 0x30D0), // ば, バ
    KanaMap(0x3073, 0x30D3), // び, ビ
    KanaMap(0x3076, 0x30D6), // ぶ, ブ
    KanaMap(0x3079, 0x30D9), // べ, ベ
    KanaMap(0x307C, 0x30DC), // ぼ, ボ

    KanaMap(0x3071, 0x30D1), // ぱ, パ
    KanaMap(0x3074, 0x30D4), // ぴ, ピ
    KanaMap(0x3077, 0x30D7), // ぷ, プ
    KanaMap(0x307A, 0x30DA), // ぺ, ペ
    KanaMap(0x307D, 0x30DD), // ぽ, ポ
};
#undef KanaMap

const std::map<KanaFix::Dakuten, QChar> KanaFix::dakuten_map = {
  {KanaFix::Dakuten::SEMI_VOICED_SOUND,   QChar(0x309A)}, // °  <- ICU/X11 [*NIX]; iOS/Android; ...
  {KanaFix::Dakuten::VOICED_SOUND_MARK,   QChar(0x309B)}, // "  <- ICU/X11 [*NIX]; ...
  {KanaFix::Dakuten::VOICED_SOUND_MARK_1, QChar(0x3099)}, // "  <- iOS/Android & some IMEs for Windows
  {KanaFix::Dakuten::VOICED_SOUND_MARK_2, QChar(0xFF9E)}, // "  <- ???
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** CLASS: KanaCompare
 * This class compares two strings kana-independent, means it doesn't matter if
 * hiragana or katakana.
 *
 * Halfwidth Katakana support is experimental and unfinished!
 *
 * NOTE: This class has 'toHiragana' and 'toKatakana', that's why I copy-pasted this class ;)
 */
class KanaCompare
{
public:
    bool compare(const QString &str1, const QString &str2) const
    {
        if (str1.size() != str2.size())
            return false;

        for (int i = 0; i < str1.size(); i++)
        {
            if (!this->compare_helper(str1.at(i).unicode(), str2.at(i).unicode()))
                return false;
        } return true;
    }

    // Convert Functions
    QChar toHiragana(const QChar &c) const
    {
        if (!this->is_kana(c.unicode()))
            return c;

        // check if char is katakana
        for (const auto &it : this->kana_map)
            if (c == it.second)
                // convert
                return QChar(it.first);
        return c;
    }

    QChar toKatakana(const QChar &c) const
    {
        if (!this->is_kana(c.unicode()))
            return c;

        // check if char is hiragana
        for (const auto &it : this->kana_map)
            if (c == it.first)
                // convert
                return QChar(it.second);
        return c;
    }

    // halfwidth-katakana with dakutens are not available in the unicode table, refer to unicode combining characters for details
    QString toHalfwidthKatakana(const QChar &c) const
    {
        if (!this->is_kana(c.unicode()))
            return QString(c);

        // convert to katakana
        QChar _c = c;
        _c = this->toKatakana(c);

        // convert to halfwidth
        for (const auto &it : this->halfwidth_katakana_map)
            if (_c == it.first)
                return it.second;
        return _c;
    }

private:
    static const std::map<QChar, QChar> kana_map;
    static const std::map<QChar, QString> halfwidth_katakana_map;

    bool compare_helper(ushort c1, ushort c2) const
    {
        // check if params are same
        if (c1 == c2) return true;

        // begin with actual compare
        for (const auto &it : this->kana_map)
        {
            for (const auto &it2 : this->halfwidth_katakana_map)
            {
                if (c1 == it.first  && c2 == it.first  ) return true;
                if (c1 == it.second && c2 == it.second ) return true;
                if (c1 == it.first  && c2 == it.second ) return true;
                if (c1 == it.second && c2 == it.first  ) return true;

                if (c1 == it.first   && QString(QChar(c2)) == it2.second ) return true;
                if (c1 == it.second  && QString(QChar(c2)) == it2.second ) return true;
            }
        }

        return false;
    }

    bool is_kana(ushort c) const
    {
        for (const auto &it : this->kana_map)
            if (c == it.first || c == it.second)
                return true;
        return false;
    }

    bool is_halfwidth_katakana(ushort c) const
    {
        for (const auto &it : this->halfwidth_katakana_map)
            if (QString(QChar(c)) == it.second)
                return true;
        return false;
    }
};

#define KanaMap(hiragana, katakana) {QChar(L##hiragana), QChar(L##katakana)}
const std::map<QChar, QChar> KanaCompare::kana_map = { // Hiragana, Katakana
    KanaMap('あ', 'ア'), KanaMap('い', 'イ'), KanaMap('う', 'ウ'), KanaMap('え', 'エ'), KanaMap('お', 'オ'),
    KanaMap('か', 'カ'), KanaMap('き', 'キ'), KanaMap('く', 'ク'), KanaMap('け', 'ケ'), KanaMap('こ', 'コ'),
    KanaMap('さ', 'サ'), KanaMap('し', 'シ'), KanaMap('す', 'ス'), KanaMap('せ', 'セ'), KanaMap('そ', 'ソ'),
    KanaMap('た', 'タ'), KanaMap('ち', 'チ'), KanaMap('つ', 'ツ'), KanaMap('て', 'テ'), KanaMap('と', 'ト'),
    KanaMap('な', 'ナ'), KanaMap('に', 'ニ'), KanaMap('ぬ', 'ヌ'), KanaMap('ね', 'ネ'), KanaMap('の', 'ノ'),
    KanaMap('は', 'ハ'), KanaMap('ひ', 'ヒ'), KanaMap('ふ', 'フ'), KanaMap('へ', 'ヘ'), KanaMap('ほ', 'ホ'),
    KanaMap('ま', 'マ'), KanaMap('み', 'ミ'), KanaMap('む', 'ム'), KanaMap('め', 'メ'), KanaMap('も', 'モ'),
    KanaMap('や', 'ヤ'), /*　　*/             KanaMap('ゆ', 'ユ'), /*　　*/             KanaMap('よ', 'ヨ'),
    KanaMap('ら', 'ラ'), KanaMap('り', 'リ'), KanaMap('る', 'ル'), KanaMap('れ', 'レ'), KanaMap('ろ', 'ロ'),
    KanaMap('わ', 'ワ'), KanaMap('ゐ', 'ヰ'), /*　　*/             KanaMap('ゑ', 'ヱ'), KanaMap('を', 'ヲ'),

    KanaMap('が', 'ガ'), KanaMap('ぎ', 'ギ'), KanaMap('ぐ', 'グ'), KanaMap('げ', 'ゲ'), KanaMap('ご', 'ゴ'),
    KanaMap('ざ', 'ザ'), KanaMap('じ', 'ジ'), KanaMap('ず', 'ズ'), KanaMap('ぜ', 'ゼ'), KanaMap('ぞ', 'ゾ'),
    KanaMap('だ', 'ダ'), KanaMap('ぢ', 'ヂ'), KanaMap('づ', 'ヅ'), KanaMap('で', 'デ'), KanaMap('ど', 'ド'),
    KanaMap('ば', 'バ'), KanaMap('び', 'ビ'), KanaMap('ぶ', 'ブ'), KanaMap('べ', 'ベ'), KanaMap('ぼ', 'ボ'),
    KanaMap('ぱ', 'パ'), KanaMap('ぴ', 'ピ'), KanaMap('ぷ', 'プ'), KanaMap('ぺ', 'ペ'), KanaMap('ぽ', 'ポ'),

    KanaMap('っ', 'ッ'), KanaMap('ん', 'ン'),
    KanaMap('ぁ', 'ァ'), KanaMap('ぃ', 'ィ'), KanaMap('ぅ', 'ゥ'), KanaMap('ぇ', 'ェ'), KanaMap('ぉ', 'ォ'),
    KanaMap('ゃ', 'ャ'), /*　　*/             KanaMap('ゅ', 'ュ'), /*　　*/             KanaMap('ょ', 'ョ'),

    KanaMap('ー', 'ｰ')
};
#undef KanaMap

#define KanaMap(katakana, halfwidth) {QChar(L##katakana), QString::fromUtf8(halfwidth)}
const std::map<QChar, QString> KanaCompare::halfwidth_katakana_map = { // Katakana, Half-width Katakana
    KanaMap('ア', "ｱ"), KanaMap('イ', "ｲ"), KanaMap('ウ', "ｳ"), KanaMap('エ', "ｴ"), KanaMap('オ', "ｵ"),
    KanaMap('カ', "ｶ"), KanaMap('キ', "ｷ"), KanaMap('ク', "ｸ"), KanaMap('ケ', "ｹ"), KanaMap('コ', "ｺ"),
    KanaMap('サ', "ｻ"), KanaMap('シ', "ｼ"), KanaMap('ス', "ｽ"), KanaMap('セ', "ｾ"), KanaMap('ソ', "ｿ"),
    KanaMap('タ', "ﾀ"), KanaMap('チ', "ﾁ"), KanaMap('ツ', "ﾂ"), KanaMap('テ', "ﾃ"), KanaMap('ト', "ﾄ"),
    KanaMap('ナ', "ﾅ"), KanaMap('ニ', "ﾆ"), KanaMap('ヌ', "ﾇ"), KanaMap('ネ', "ﾈ"), KanaMap('ノ', "ﾉ"),
    KanaMap('ハ', "ﾊ"), KanaMap('ヒ', "ﾋ"), KanaMap('フ', "ﾌ"), KanaMap('ヘ', "ﾍ"), KanaMap('ホ', "ﾎ"),
    KanaMap('マ', "ﾏ"), KanaMap('ミ', "ﾐ"), KanaMap('ム', "ﾑ"), KanaMap('メ', "ﾒ"), KanaMap('モ', "ﾓ"),
    KanaMap('ヤ', "ﾔ"), /*  　*/            KanaMap('ユ', "ﾕ"), /*  　*/            KanaMap('ヨ', "ﾖ"),
    KanaMap('ラ', "ﾗ"), KanaMap('リ', "ﾘ"), KanaMap('ル', "ﾙ"), KanaMap('レ', "ﾚ"), KanaMap('ロ', "ﾛ"),
    KanaMap('ワ', "ﾜ"), /*　　*/            /*　　*/            /*　　*/             KanaMap('ヲ', "ｦ"),

    KanaMap('ガ', "ｶﾞ"), KanaMap('ギ', "ｷﾞ"), KanaMap('グ', "ｸﾞ"), KanaMap('ゲ', "ｹﾞ"), KanaMap('ゴ', "ｺﾞ"),
    KanaMap('ザ', "ｻﾞ"), KanaMap('ジ', "ｼﾞ"), KanaMap('ズ', "ｽﾞ"), KanaMap('ゼ', "ｾﾞ"), KanaMap('ゾ', "ｿﾞ"),
    KanaMap('ダ', "ﾀﾞ"), KanaMap('ヂ', "ﾁﾞ"), KanaMap('ヅ', "ﾂﾞ"), KanaMap('デ', "ﾃﾞ"), KanaMap('ド', "ﾄﾞ"),
    KanaMap('バ', "ﾊﾞ"), KanaMap('ビ', "ﾋﾞ"), KanaMap('ブ', "ﾌﾞ"), KanaMap('ベ', "ﾍﾞ"), KanaMap('ボ', "ﾎﾞ"),
    KanaMap('パ', "ﾊﾟ"), KanaMap('ピ', "ﾋﾟ"), KanaMap('プ', "ﾌﾟ"), KanaMap('ペ', "ﾍﾟ"), KanaMap('ポ', "ﾎﾟ"),

    KanaMap('ッ', "ｯ"), KanaMap('ン', "ﾝ"),
    KanaMap('ァ', "ｧ"), KanaMap('ィ', "ｨ"), KanaMap('ゥ', "ｩ"), KanaMap('ェ', "ｪ"), KanaMap('ォ', "ｫ"),
    KanaMap('ャ', "ｬ"), /*　　*/            KanaMap('ュ', "ｭ"), /*  　*/            KanaMap('ョ', "ｮ"),

    KanaMap('ー', "ｰ")
};
#undef KanaMap

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QStringList UniversalJapaneseKanaLookup::processString(const QString &str) const
{
    QString data = str;

    // fix (broken?) dakuten coding
    static const KanaFix kanaFix;
    kanaFix.fixChar(data);

    QStringList searchMap;
    int size = 0;

    // append fixed string, if you use a "shitty" IME, the original file path is also included in the list
    // so it doesn't really matter ;)
    searchMap.append(data);

    // create a string for only hiragana and only katakana, for a more comfortable lookup
    static const KanaCompare kanaCompare;

    QString hiragana = data;
    QString katakana = data;
    QString halfwidth = data;

    size = hiragana.size();
    for (int i = 0; i < size; i++)
        hiragana.replace(i, 1, kanaCompare.toHiragana(hiragana.at(i)));

    size = katakana.size();
    for (int i = 0; i < size; i++)
        katakana.replace(i, 1, kanaCompare.toKatakana(katakana.at(i)));

    // string size changes
    for (int i = 0; i < halfwidth.size(); i++)
        halfwidth.replace(i, 1, kanaCompare.toHalfwidthKatakana(halfwidth.at(i)));

    // append the strings
    searchMap.append(hiragana);
    searchMap.append(katakana);
    searchMap.append(halfwidth);

    // free some memory
    data.clear();
    hiragana.clear();
    katakana.clear();
    halfwidth.clear();
    size = 0;

    return searchMap;
}
