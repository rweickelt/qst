#include "dependency.h"

Dependency::Dependency() : d(new DependencyData()) {}

Dependency::Dependency(const Dependency& other) : d(other.d) {}

Dependency& Dependency::operator=(const Dependency& other) {
    this->d = other.d;
    return *this;
}
