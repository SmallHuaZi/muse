#ifndef LIB_UTIL_ENUMBITS_HPP
#define LIB_UTIL_ENUMBITS_HPP

#include <type_traits>

#define BIT(N)  (1ULL << N)

#define MUSE_ENABLE_ENUMBITS(T)    \
    [[maybe_unused]] static auto muse_enable_enumbits(T) -> int;\

#define MUSE_ENABLE_ENUMBITS_INNER(T)    \
    friend auto muse_enable_enumbits(T) -> int;\

template <typename Enum>
static constexpr auto muse_enable_enumbits(Enum e) -> char;

template <typename Enum>
static constexpr bool const MuseEnumBitsEnabledV = sizeof(muse_enable_enumbits(std::declval<Enum>())) == sizeof(int);

template <typename Enum>
constexpr auto operator|(Enum lhs, Enum rhs) 
    -> std::enable_if_t<MuseEnumBitsEnabledV<Enum>, Enum>
{
    typedef std::underlying_type_t<Enum>     Underlying;
    return static_cast<Enum> (
        static_cast<Underlying>(lhs) |
        static_cast<Underlying>(rhs)
    );
}

template <typename Enum>
constexpr auto operator&(Enum lhs, Enum rhs) 
    -> std::enable_if_t<MuseEnumBitsEnabledV<Enum>, Enum>
{
    typedef std::underlying_type_t<Enum>     Underlying;
    return static_cast<Enum> (
        static_cast<Underlying>(lhs) &
        static_cast<Underlying>(rhs)
    );
}

template <typename Enum>
constexpr auto operator~(Enum e) 
    -> std::enable_if_t<MuseEnumBitsEnabledV<Enum>, Enum>
{
    typedef std::underlying_type_t<Enum>     Underlying;
    return static_cast<Enum>(~static_cast<Underlying>(e));
}

template<typename Enum>
constexpr auto operator|=(Enum& lhs, Enum rhs) 
    -> std::enable_if_t<MuseEnumBitsEnabledV<Enum>, std::add_lvalue_reference_t<Enum>>
{
    lhs = lhs | rhs;
    return lhs;
}

template<typename Enum>
constexpr auto operator&=(Enum& lhs, Enum rhs) 
    -> std::enable_if_t<MuseEnumBitsEnabledV<Enum>, std::add_lvalue_reference_t<Enum>>
{
    lhs = lhs & rhs;
    return lhs;
}

template<typename Enum>
constexpr auto operator!(Enum lhs) 
    -> std::enable_if_t<MuseEnumBitsEnabledV<Enum>, bool>
{
    typedef std::underlying_type_t<Enum>     Underlying;
    return !Underlying(lhs);
}


#endif // LIB_UTIL_ENUMBITS_HPP