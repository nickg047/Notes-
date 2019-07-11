#include "color.h"

std::ostream& Color::operator<<(std::ostream& lhs, const Color::Modifier& rhs){
    return lhs << "\033[" << rhs.getCode() << "m";
}

Color::Code Color::Modifier::getCode() const{
    return code;
}

std::string Color::operator+(const std::string& lhs, const Color::Modifier& rhs){
    std::ostringstream stream;
    stream << "\033[" << rhs.getCode() << "m" << lhs;
    return stream.str();
}

std::string Color::Modifier::operator+(const std::string& rhs) const{
    std::ostringstream stream;
    stream << "\033[" << this->code << "m" << rhs;
    return stream.str();
}
