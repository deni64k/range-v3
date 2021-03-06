/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_ACTION_PUSH_FRONT_HPP
#define RANGES_V3_ACTION_PUSH_FRONT_HPP

#include <utility>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/action/action.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/detail/with_braced_init_args.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/utility/static_const.hpp>

#include <range/v3/detail/disable_warnings.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{

    /// \cond
    namespace adl_push_front_detail
    {
        template<typename Cont, typename T>
        using push_front_t = decltype(static_cast<void>(
            unwrap_reference(std::declval<Cont &>()).push_front(std::declval<T>())));

        template<typename Cont, typename Rng>
        using insert_t = decltype(static_cast<void>(
            ranges::insert(std::declval<Cont &>(), std::declval<iterator_t<Cont>>(),
                           std::declval<Rng>())));

        template<typename Cont, typename T>
        auto push_front(Cont && cont, T && t) -> CPP_ret(push_front_t<Cont, T>)( //
            requires lvalue_container_like<Cont> &&
            (!range<T>)&&constructible_from<range_value_t<Cont>, T>)
        {
            unwrap_reference(cont).push_front(static_cast<T &&>(t));
        }

        template<typename Cont, typename Rng>
        auto push_front(Cont && cont, Rng && rng) -> CPP_ret(insert_t<Cont, Rng>)( //
            requires lvalue_container_like<Cont> && range<Rng>)
        {
            ranges::insert(cont, begin(cont), static_cast<Rng &&>(rng));
        }

        // clang-format off
        template<typename Rng, typename T>
        CPP_concept_fragment(can_push_front_frag_,
            requires(Rng && rng, T && t) //
            (
                push_front(rng, (T &&) t)
            )
        );
        template<typename Rng, typename T>
        CPP_concept can_push_front_ =
            CPP_fragment(adl_push_front_detail::can_push_front_frag_, Rng, T);
        // clang-format on

        struct push_front_fn
        {
            template<typename T>
            constexpr auto operator()(T && val) const
            {
                return make_action_closure(
                    bind_back(push_front_fn{}, static_cast<T &&>(val)));
            }

            template<typename T>
            constexpr auto operator()(std::initializer_list<T> val) const
            {
                return make_action_closure(bind_back(push_front_fn{}, val));
            }

            template<typename T>
            constexpr auto CPP_fun(operator())(T & t)( //
                const requires range<T &>)
            {
                return make_action_closure(
                    bind_back(push_front_fn{}, detail::reference_wrapper_<T>(t)));
            }

            template<typename Rng, typename T>
            auto operator()(Rng && rng, T && t) const //
                -> CPP_ret(Rng)(                      //
                    requires input_range<Rng> && can_push_front_<Rng, T> &&
                    (range<T> || constructible_from<range_value_t<Rng>, T>))
            {
                push_front(rng, static_cast<T &&>(t));
                return static_cast<Rng &&>(rng);
            }

            template<typename Rng, typename T>
            auto operator()(Rng && rng, std::initializer_list<T> t) const //
                -> CPP_ret(Rng)(                                          //
                    requires input_range<Rng> &&                          //
                        can_push_front_<Rng, std::initializer_list<T>> &&
                            constructible_from<range_value_t<Rng>, T const &>)
            {
                push_front(rng, t);
                return static_cast<Rng &&>(rng);
            }

            /// \cond
            template<typename Rng, typename T>
            auto operator()(Rng && rng, detail::reference_wrapper_<T> r) const
                -> invoke_result_t<push_front_fn, Rng, T &>
            {
                return (*this)(static_cast<Rng &&>(rng), r.get());
            }
            /// \endcond
        };
    } // namespace adl_push_front_detail
    /// \endcond

    namespace actions
    {
        RANGES_INLINE_VARIABLE(adl_push_front_detail::push_front_fn, push_front)
    } // namespace actions

    using actions::push_front;

    /// @}
} // namespace ranges

#include <range/v3/detail/reenable_warnings.hpp>

#endif
