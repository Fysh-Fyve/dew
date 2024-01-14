/**
 * Copyright (C) 2024 Charles Ancheta
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \file type.h
 */
#ifndef DEW_TYPE_H_
#define DEW_TYPE_H_

#include <string_view>

namespace dew {
using DataType = std::string_view;

constexpr DataType INT8 = "i8";
constexpr DataType INT16 = "i16";
constexpr DataType INT32 = "i32";
constexpr DataType UINT8 = "u8";
constexpr DataType UINT16 = "u16";
constexpr DataType UINT32 = "u32";
} // namespace dew

#endif // !DEW_TYPE_H_
