#pragma once


#include "MatrixBase.h" // IWYU pragma: export
#include "Matrix.h"


namespace ad {
namespace math {


/// \brief Base class for all vectors (i.e. matrices with exactly one row)
template <class T_derived, int N_dimension, class T_number>
class Vector : public MatrixBase<T_derived, 1, N_dimension, T_number>
{
    typedef MatrixBase<T_derived, 1, N_dimension, T_number> base_type;
    using base_type::base_type;

public:
    // Public so free functions can use it
    using base_type::should_noexcept;

    static constexpr std::size_t Dimension{N_dimension};

    /// \brief Constructor from a head vector of lower dimension, with explicit tail element to complete.
    ///
    /// \note Currently accept a head vector of different logic type (e.g. `Position` vs `Vec`) as long as
    /// the element types exactly match. This might be later restricted if the need arises.
    template <class T_otherDerived,
              int N_headDimension,
              class... VT_tailElements,
              class /*enabler*/ = std::enable_if_t<(N_headDimension < N_dimension)
                                                   && sizeof...(VT_tailElements) == (N_dimension - N_headDimension)>>
    constexpr Vector(const Vector<T_otherDerived, N_headDimension, T_number> & aHead, VT_tailElements && ... aTail)
    noexcept(should_noexcept) :
            Vector{aHead, std::make_index_sequence<N_headDimension>(), std::forward<VT_tailElements>(aTail)...}
    {}

    /// \brief Constructor from another vector of larger dimension (swizzling constructor).
    ///
    /// \note Currently accept a larger vector of different logic type (e.g. `Position` vs `Vec`) as long as
    /// the element types exactly match. This might be later restricted if the need arises.
    template <class T_otherDerived,
              int N_otherDimension,
              class /*enabler*/ = std::enable_if_t<(N_otherDimension > N_dimension)>>
    constexpr explicit Vector(const Vector<T_otherDerived, N_otherDimension, T_number> & aLarger)
    noexcept(should_noexcept) :
            Vector{aLarger, std::make_index_sequence<N_dimension>()}
    {}


    /// @brief Constructor from a matrix row.
    template <int N_cols, class T_rowNumber>
    requires std::is_same_v<std::remove_const_t<T_rowNumber>, T_number>
    explicit Vector(const detail::RowBase<N_cols, T_rowNumber> & aRow) noexcept(should_noexcept) :
        Vector{aRow.data(), std::make_index_sequence<N_dimension>()}
    {}


    template <template <int, class> class TT_derivedVector, class T_targetNumber=T_number>
    constexpr TT_derivedVector<N_dimension, T_targetNumber> as() const;

    constexpr T_number &operator[](std::size_t aColumn);
    constexpr T_number operator[](std::size_t aColumn) const;

    constexpr T_derived & operator*=(const Matrix<N_dimension, N_dimension, T_number> &aRhs);
    using base_type::operator*=;

    /// \brief Dot product
    constexpr T_number dot(const Vector &aRhs) const;

    /// \brief Outer product
    ///
    /// see: https://en.wikipedia.org/wiki/Outer_product
    ///
    /// \note It returns a matrix, so it does not try to restrict to vector of the same derived type.
    template <class T_rightDerived, int N_rightDimension>
    constexpr Matrix<N_dimension, N_rightDimension, T_number>
    outer(const Vector<T_rightDerived, N_rightDimension, T_number> & aRhs) const;

    /// \brief Vector magnitude squared (faster than normal magnitudes)
    constexpr T_number getNormSquared() const;

    // Implementer's note: Not constexpr, because math functions are not (relies on std::sqrt)
    /// \brief Vector magnitude
    /*constexpr*/ T_number getNorm() const;

    // Implementer's note: Not constexpr, because getNorm() is not
    // TODO Ad 2022/03/11 Should return the Unit version of T_derived
    // In fact, does it make sense for anything else than Vec derived type?
    /// \brief Compound normalization
    /*constexpr*/ T_derived & normalize();

private:
    // Implementation for the public constructor taking a head vector of lower dimension.
    template <class T_vector, std::size_t... VN_indices, class... VT_tailElements>
    constexpr Vector(const T_vector & aHead,
                     std::index_sequence<VN_indices ...>,
                     VT_tailElements && ... aTail) noexcept(should_noexcept);
};

template <class T_derived, int N_dimension, class T_number>
constexpr T_derived operator*(const Vector<T_derived, N_dimension, T_number> aLhs,
                              const Matrix<N_dimension, N_dimension, T_number> &aRhs);


/***
 * Specializations
 ***/

#define ENABLER(condition) \
    template <int N=N_dimension, class = std::enable_if_t<(N condition && N==N_dimension)>>

#define ACCESSOR_DIMENSION(symbol, dimension) \
    ENABLER(>= dimension)           \
    constexpr T_number & symbol()/* requires (N_dimension>=dimension)*/ \
    {static_assert(N_dimension>=dimension, "Disabled when dimensions < " #dimension); return this->at(dimension-1);} \
                                    \
    ENABLER(>= dimension)           \
    constexpr T_number symbol() const/* requires (N_dimension>=dimension)*/ \
    {static_assert(N_dimension>=dimension, "Disabled when dimensions <" #dimension); return this->at(dimension-1);}

#define SWIZZLE_DIMENSION(dimension, type, function, ...)   \
    ENABLER(>= dimension)                                   \
    constexpr type<dimension-1, T_number> function() const  \
    {return {__VA_ARGS__};}



#define BASE Vector<Vec<N_dimension, T_number>, N_dimension, T_number>
template <int N_dimension, class T_number=real_number>
class Vec : public BASE
{
    typedef BASE base_type;
    using base_type::base_type;

public:
    template<class T>
    using derived_type = Vec<N_dimension, T>;

    // This approach has drawbacks:
    // * The error can be hard to read, with many lines of template substition error
    // * It is not fool-proof, because client code can still give explicit value for function template arguments
    // A good solution seems to be `requires` keyword introduced in C++20,
    // but we do with a mix of enbable_if and static_assert for the moment.
    // see: https://stackoverflow.com/q/58068206/1027706
    //
    //template <int N=N_dimension, class = std::enable_if_t<(N>1 && N==N_dimension)>> T_number & y()
    //{return this->at(1);}

    ACCESSOR_DIMENSION(x, 1)
    ACCESSOR_DIMENSION(y, 2)
    ACCESSOR_DIMENSION(z, 3)
    ACCESSOR_DIMENSION(w, 4)

    SWIZZLE_DIMENSION(5, Vec, xyzw, x(), y(), z(), w());
    SWIZZLE_DIMENSION(4, Vec, xyz,  x(), y(), z());
    SWIZZLE_DIMENSION(3, Vec, xy,   x(), y());

    /// \todo Could be extended to all dimensions >= 3?
    Vec & crossAssign(const Vec &aRhs) /*requires(N_dimension==3)*/;
    Vec cross(const Vec &aRhs) /*requires(N_dimension==3)*/;
};
#undef BASE


template <int N_dimension, class T_number>
class is_vec<Vec<N_dimension, T_number>> : public std::true_type
{};


/// \brief Unit vector class, ensuring an instance of this class has norm 1.
template <int N_dimension, class T_number=real_number>
class UnitVec : public Vec<N_dimension, T_number>
{
    typedef Vec<N_dimension, T_number> base_type;

    // explicit is a workaround to prevent UnitVec from being an aggregate type
    // (otherwise, a deleted or defaulted ctor would still allow for aggregate initialization)
    // see: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1008r0.pdf paragraph 1.3
    explicit UnitVec() = delete;

    struct already_normalized {};
    constexpr UnitVec(base_type aNormalizedVec, already_normalized) : base_type{std::move(aNormalizedVec)}
    {}

public:
    // Cannot be used in a constexpr context atm, because normalize is not (because std::sqrt is not).
    explicit constexpr UnitVec(base_type aVec) : base_type{aVec.normalize()}
    {}

    static constexpr UnitVec MakeFromUnitLength(base_type aNormalizedVec)
    {
        return UnitVec{aNormalizedVec, already_normalized{}};
    }

    constexpr UnitVec operator-() const
    {
        return {base_type::operator-(), already_normalized{}};
    }
};


#define BASE Vector<Position<N_dimension, T_number>, N_dimension, T_number>
template <int N_dimension, class T_number=real_number>
class Position : public BASE
{
    typedef BASE base_type;
    using base_type::base_type;

public:
    template<class T>
    using derived_type = Position<N_dimension, T>;

    ACCESSOR_DIMENSION(x, 1)
    ACCESSOR_DIMENSION(y, 2)
    ACCESSOR_DIMENSION(z, 3)
    ACCESSOR_DIMENSION(w, 4)

    SWIZZLE_DIMENSION(5, Position, xyzw, x(), y(), z(), w());
    SWIZZLE_DIMENSION(4, Position, xyz,  x(), y(), z());
    SWIZZLE_DIMENSION(3, Position, xy,   x(), y());
};
#undef BASE


template <int N_dimension, class T_number>
class is_position<Position<N_dimension, T_number>> : public std::true_type
{};

template <int N_dimension, class T_number>
struct addition_trait<Position<N_dimension, T_number>, Position<N_dimension, T_number>>
                     : public std::false_type
{};

template <int N_dimension, class T_number>
struct addition_trait<Position<N_dimension, T_number>, Vec<N_dimension, T_number>>
                     : public std::true_type
{};

template <int N_dimension, class T_number>
Vec<N_dimension, T_number> operator-(Position<N_dimension, T_number> aLhs,
                                     Position<N_dimension, T_number> aRhs)
{
    Vec<N_dimension, T_number> result = static_cast<Vec<N_dimension, T_number>>(aLhs);
    return (result -= static_cast<Vec<N_dimension, T_number>>(aRhs));
}

#define BASE Vector<Size<N_dimension, T_number>, N_dimension, T_number>
template <int N_dimension, class T_number=real_number>
class Size : public BASE
{
    /// \todo Disable most of the available functions
    typedef BASE base_type;
    using base_type::base_type;

public:
    template<class T>
    using derived_type = Size<N_dimension, T>;
    
    ACCESSOR_DIMENSION(width,  1)
    ACCESSOR_DIMENSION(height, 2)
    ACCESSOR_DIMENSION(depth,  3)

    template <int N=N_dimension, class = std::enable_if_t<(N==2 && N==N_dimension)>>
    T_number area() const/* requires (N_dimension==2)*/
    {static_assert(N_dimension==2, "Disabled when dimension != 2"); return width()*height();}

    template <int N=N_dimension, class = std::enable_if_t<(N==3 && N==N_dimension)>>
    T_number volume() const/* requires (N_dimension==3)*/
    {static_assert(N_dimension==3, "Disabled when dimension != 3"); return width()*height()*depth();}
};
#undef BASE


template <int N_dimension, class T_number>
class is_size<Size<N_dimension, T_number>> : public std::true_type
{};


#undef ACCESSOR_DIMENSION
#undef ENABLER


}} // namespace ad::math


#include "Vector-impl.h"
