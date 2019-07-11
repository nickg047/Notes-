#ifndef COLOR_H
#define COLOR_H

#include <ostream>
#include <sstream>
#include <string>
namespace Color {
    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49
    };
    class Modifier {
        private:
            Code code;
        public:
            inline Modifier(Code pCode) : code(pCode) {}
            friend std::ostream& operator<<(std::ostream&, const Modifier&);
            std::string operator+(const std::string&) const;
            friend std::string operator+(const std::string&, const Modifier&);
            Code getCode() const;
    };
    
}

#endif
