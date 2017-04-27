#ifndef S3D_UTILITIES_RULE_OF_FIVE_H
#define S3D_UTILITIES_RULE_OF_FIVE_H

#include <gsl/gsl>

template <class Interface>
class rule_of_five_interface {
 public:
  // todo(hugbed): virtual functions probably prevent empty base class
  // optimization
  virtual gsl::owner<Interface*> clone() const = 0;
  virtual ~rule_of_five_interface() = default;
  rule_of_five_interface(const rule_of_five_interface&) = delete;
  rule_of_five_interface& operator=(const rule_of_five_interface&) = delete;
  rule_of_five_interface(rule_of_five_interface&& /*unused*/) noexcept;
  rule_of_five_interface& operator=(rule_of_five_interface&& /*unused*/) noexcept;

 protected:
  constexpr rule_of_five_interface() = default;
};

template <class Interface>
inline rule_of_five_interface<Interface>::rule_of_five_interface(
    rule_of_five_interface<Interface>&&) noexcept = default;

template <class Interface>
inline rule_of_five_interface<Interface>& rule_of_five_interface<Interface>::operator=(
    rule_of_five_interface<Interface>&&) noexcept = default;

#endif  // S3D_UTILITIES_RULE_OF_FIVE_H
