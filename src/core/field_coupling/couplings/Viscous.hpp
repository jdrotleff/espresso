/*
 * Copyright (C) 2010-2022 The ESPResSo project
 *
 * This file is part of ESPResSo.
 *
 * ESPResSo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ESPResSo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CORE_CONSTRAINTS_DETAIL_VISCOUS_HPP
#define CORE_CONSTRAINTS_DETAIL_VISCOUS_HPP

#include <utils/Vector.hpp>
#include <unordered_map>

namespace FieldCoupling {
namespace Coupling {
class Viscous {
  std::unordered_map<int, double> m_scales;
  double m_default;

public:
  static constexpr bool is_linear = true;

  /* Keep a simple constructor for backward compatibility */
  Viscous(double gamma) : m_scales(), m_default(gamma) {}

  /* Allow initialization with per-particle scales + default */
  template <typename ScalesRef>
  Viscous(ScalesRef &&scales, double default_val)
      : m_scales(std::forward<ScalesRef>(scales)), m_default(default_val) {}

  /* Backwards-compatible accessor */
  double &gamma() { return m_default; }
  double const &gamma() const { return m_default; }

  std::unordered_map<int, double> &particle_scales() { return m_scales; }
  std::unordered_map<int, double> const &particle_scales() const {
    return m_scales;
  }

private:
  template <typename Particle> double scale(Particle const &p) const {
    // Lookup by particle type only. The friction/gamma is defined per type
    // in this project, so we use the particle type as the key into
    // `m_scales`. If no entry for the type exists, fall back to the
    // default gamma value.
    auto it_type = m_scales.find(p.type());
    if (it_type != m_scales.end()) {
      return it_type->second;
    }

    return m_default;
  }

public:
  template <typename Particle>
  Utils::Vector3d operator()(Particle const &p,
                             Utils::Vector3d const &field) const {
    const double s = scale(p);
    return s * (field - p.v());
  }
};
} // namespace Coupling
} // namespace FieldCoupling

#endif
