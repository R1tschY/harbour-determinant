#include "stringutils.h"

#include <QQmlEngine>
#include <QtQml>
#include <QString>
#include <algorithm>

namespace Det {

uint VARIATION_SELECTOR_16 = 0xFE0F;
uint ZERO_WIDTH_JOINER = 0x200D;

StringUtils::StringUtils(QObject *parent) : QObject(parent)
{

}

void StringUtils::registerType()
{
    qmlRegisterSingletonType<StringUtils>(
                "Determinant.Qml", 0, 2, "StringUtils",
                [](QQmlEngine*, QJSEngine*) -> QObject* {
        return new StringUtils();
    });
}

int StringUtils::getEmoijChainLength(const QString& s) const {
    int result = 0;
    for (int i = 0; i < s.size(); i += 1) {
        uint c = s[i].unicode();
        if (QChar::isHighSurrogate(c)) {
            i += 1;
            if (i < s.size()) {
                c = QChar::surrogateToUcs4(c, s[i].unicode());
                if (QChar::isSymbol(c)) {
                    result += 1;
                    continue;
                }

                return 0;
            } else {
                // missing low surrogate
                return 0;
            }
        } else {
            if (QChar::isSymbol(c)) {
                result += 1;
                continue;
            }

            if (c == VARIATION_SELECTOR_16) {
                continue;
            }

            if (c == ZERO_WIDTH_JOINER) {
                result -= 1;
                continue;
            }

            return 0;
        }
    }
    return result;
}

bool StringUtils::isSymbol(const QString &input) const
{
    return std::all_of(input.begin(), input.end(), [](QChar c) {
        return c.isSymbol();
    });
}

// // Heart: Variation_Selector_16
// getEmoijChainLength("❤️"); // "\u2764\ufe0f";

// // Thumbs Up: with surrogate
// getEmoijChainLength("👍"); // "\u1F44D";

// // Kiss: Woman, Man
// getEmoijChainLength("🔪"); // "\u1F470\u200D\u2640\uFE0F";

// // Woman with Veil
// getEmoijChainLength("👰‍♀️"); // "\u1F469\u200D\u2764\uFE0F\u200D\u1F48B\u200D\u1F468";

// // Person: Dark Skin Tone: Skin
// getEmoijChainLength("🧑🏿"); // "\u1F469\u200D\u2764\uFE0F\u200D\u1F48B\u200D\u1F468";

// // Pinching Hand
// getEmoijChainLength("🤏"); // "\u1F90F";

// // Flag: Netherlands
// getEmoijChainLength("🇳🇱"); // "\u1F1F3\u1F1F1";

} // namespace Det
