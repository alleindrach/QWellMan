#ifndef DTO_H
#define DTO_H

#define AUTO_PROPERTY(TYPE, NAME) \
    Q_PROPERTY(TYPE NAME READ NAME WRITE NAME NOTIFY NAME ## Changed ) \
    public: \
       TYPE NAME() const { return m_ ## NAME ; } \
       void NAME(TYPE value) { \
          if (m_ ## NAME == value)  return; \
          m_ ## NAME = value; \
          emit NAME ## Changed(value); \
        } \
       Q_SIGNAL void NAME ## Changed(TYPE value);\
    private: \
       TYPE m_ ## NAME;


#define DECLARE(x)  \
    Q_DECLARE_METATYPE(x)  \
    Q_DECLARE_METATYPE(x*) \
    Q_DECLARE_METATYPE(QList<x*>)

#endif // DTO_H
