#ifndef STDHASH_H
#define STDHASH_H

#include <QHash>
#include <QString>
#include <functional>

namespace std {
  template<> struct hash<QString> {
    std::size_t operator()(const QString& s) const noexcept {
      return (size_t) qHash(s);
    }
  };
}

#endif // STDHASH_H
