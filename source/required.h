/*
	Parrot
	Copyright (C) 2023 Robin Southern https://github.com/betajaen/parrot

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#if defined(PARROT_AMIGA)
#include "platform/amiga/types.h"
struct TagItem;
#else
struct TagItem {
	Parrot::Uint32 ti_Tag;
	Parrot::Uint32 ti_Data;
};
#endif

#include <new>

namespace Parrot {
	void Assert(ConstCString file, Uint32 line);
	void ShowAlert(ConstCString str);
}

#define PARROT_ASSERT(COND) do { if (!(COND)) { ::Parrot::Assert(__FILE__, __LINE__); } } while(0)
#define PARROT_ERROR(STR) ::Parrot::ShowAlert(STR)

namespace Parrot
{
	typedef Uint8			Byte;
	typedef Uint16			Tag16;
	typedef Uint32			Tag32;

	enum class GameName
	{
		None = 0,
		Maniac = 1
	};


	constexpr Tag16 MakeTag16(Byte A, Byte B) {
		return ((Uint16) (A)  << 8 | (Uint16) B);
	}

	constexpr Tag32 MakeTag32(Byte A, Byte B, Byte C, Byte D) {
		return ((Uint32) (A)<<24 | (Uint32) (B)<<16 | (Uint32) (C)<<8 | (Uint32) (D));
	}
	
	struct TagPair16 {

		Tag16 Tag;
		Uint32 Start;
		Uint32 End;

		template<Byte A, Byte B>
		constexpr bool IsTag() const {
			return Tag == MakeTag16(A, B);
		}
		
		bool IsTag(Tag32 otherTag) const {
			return Tag == otherTag;
		}

		Uint32 Length() const {
			return End - Start - 6;
		}
	
	};

	struct TagPair32 {

		Tag32 Tag;
		Uint32 Start;
		Uint32 End;

		template<Byte A, Byte B, Byte C, Byte D>
		constexpr bool IsTag() const {
			return Tag == MakeTag32(A, B, C, D);
		}
		
		bool IsTag(Tag32 otherTag) const {
			return Tag == otherTag;
		}

		Uint32 Length() const {
			return End - Start - 8;
		}
	
	};
	
	template<typename T, Size Capacity>
    struct Array {
    private:
        T Items[Capacity];
    public:

        Array() = default;
        ~Array() = default;

        constexpr Size GetCapacity() const {
            return Capacity;
        }

        inline T& operator[](Size Index) {
            PARROT_ASSERT(Index < Capacity);
            return Items[Index];
        }

        inline const T& operator[](Size Index) const {
            PARROT_ASSERT(Index < Capacity);
            return Items[Index];
        }
    };

    template<typename T>
    union BytesUnion {
        T Value;
        Byte Bytes[sizeof(T)];
    };

	template<typename T>
	struct Opt {
		bool HasValue;
		T Value;

		Opt(T value) : Value(value), HasValue(true) {}
		Opt() : Value{}, HasValue(false) {}
		~Opt() = default;

		operator bool() const {
			return HasValue;
		}

	};

	template<Size Capacity>
	struct TagList {
	private:
		Size It;
		Uint32 Tags[Capacity*2];
	public:

		TagList() : It(0) {
			Tags[0] = 0;
		}

		TagItem* GetTags() const {
			return (TagItem*) ((Uint32*)&Tags[0]);
		}

		void Done() {
			Tags[It++] = 0;
		}

		void Value(Uint32 tag, Uint32 value) {
			PARROT_ASSERT(It < (Capacity*2));
			Tags[It++] = tag;
			Tags[It++] = value;
		}

		template<typename T>
		void Ptr(Uint32 tag, T* value) {
			Value(tag, (Uint32) (value));
		}

	};
	
}
