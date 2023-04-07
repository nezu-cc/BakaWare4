#pragma once

using hash_t = uint_fast32_t;

template<class ty>
concept string_like = requires(ty t) {
    t.data();
    t.substr();
};

inline namespace crypt {

    namespace fnv1a {

        constexpr hash_t basis = 0x811c9dc5;
        constexpr hash_t prime = 0x1000193;

        template<std::integral ch = char>
        constexpr hash_t hash(const ch* str) noexcept
        {
            const auto len = [str]() {
                size_t i{ };
                while (str[i])
                    i++;
                return i;
            }();

            auto hashed = basis;
            for (size_t i{ }; i < len; i++) {
                hashed ^= str[i];
                hashed *= prime;
            }
            return hashed;
        }

        template<string_like st = std::string_view>
        inline hash_t hash(const st& str) noexcept
        {
            return hash(str.data());
        }

        namespace literals {

            constexpr auto operator""_hash(const char* str, size_t len) noexcept
            {
                return hash(str);
            }

            constexpr auto operator""_hash(const wchar_t* str, size_t len) noexcept
            {
                return hash(str);
            }

        }

    }

}

using namespace fnv1a::literals;
