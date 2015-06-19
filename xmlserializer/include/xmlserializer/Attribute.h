/*
 * Copyright (c) 2015, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include <convert.hpp>

#include <list>
#include <string>
#include <memory>
#include <functional>
#include <stdexcept>
#include <utility>

namespace core
{
namespace xml
{
namespace binding
{

/** Data member setter to use during deserialization
 *
 * @tparam the type of the data to retrieve
 */
template <typename T> using Setter = std::function<void(T)>;
template <typename T> using Getter = std::function<T()>;
template <typename T> using Binder = std::pair<Getter<T>, Setter<T> >;

/** Helper for accessor/mutator pair creation
 *
 * @tparam T the type of the attribute to bind
 * @param[in] attribute variable to bind
 * @return a Binder variable which contains an accessor and a mutator to attribute parameter
 */
template <typename T>
Binder<T> makeBinder(T &attribute)
{
    return make_pair(Getter<T>([&attribute](){return attribute;}),
                     Setter<T>([&attribute](T value){attribute = value;}));
}


/** Type deduction helper structure
 *
 * @see Attribute
 */
template <typename T> struct Type {};

// @{
/** Available type List */
template<> struct Type<int> { using type = int; };
template<> struct Type<long> { using type = long; };
template<> struct Type<long long> { using type = long long; };
template<> struct Type<unsigned> { using type = unsigned; };
template<> struct Type<short unsigned> { using type = short unsigned; };
template<> struct Type<unsigned long> { using type = unsigned long; };
template<> struct Type<unsigned long long> { using type = unsigned long long; };
template<> struct Type<float> { using type = float; };
template<> struct Type<double> { using type = double; };
template<> struct Type<long double> { using type = long double; };
template<> struct Type<bool> { using type = bool; };
template<> struct Type<std::string> { using type = std::string; };
// @}

namespace details
{
/** Attribute Implementation forward declaration */
template <typename T>
class AttributeImpl;
} /** details namespace */

/** Xml Attribute representation.
 * Allows to retrieve the data in an xml file and to set it in the right place
 * This is a general facade which allows to store any type of attributes in the same container
 *
 * @tparam the type of the data to retrieve
 */
class Attribute
{
public:

    /** Define a complete attribute. An attribute should contains methods to store and retrieves
     *  data of an Object. This is done using Getter and Setter object.
     *
     * @tparam[in] Type type of the binded attribute
     * @tparam[in] GetterType signature of the getter used to get data from the binded attribute
     * @tparam[in] SetterType signature of the setter used to set data in the binded attribute
     *
     * @param[in] name the name of the corresponding XML attribute
     * @param[in] type help structure containing the type of the binded attribute
     *                 C++ cannot deduce the type signature of a lambda, moreover there is no
     *                 'make_function' method. Using this variable allows to help the compiler
     *                 to deduce the type of getter and setter parameter which will be wrapped
     *                 in Getter and Setter object later.
     *                 This is only syntactic sugar to avoid the type to create Getter and Setter
     *                 object for each attribute.
     * @param[in] getter the getter method of the binded data
     * @param[in] setter the setter method of the binded data
     */
    template<class Type, typename GetterType, typename SetterType>
    Attribute(const std::string &name, Type &&/**type*/, GetterType &&getter, SetterType &&setter) :
        mAttributeImpl{new details::AttributeImpl<typename Type::type>(name,
                                                              Getter<typename Type::type>(getter),
                                                              Setter<typename Type::type>(setter))}
    {
    }

    /** Define a complete attribute. An attribute should contains methods to store and retrieves
     *  data of an Object. This is done using Binder object.
     *
     * @tparam[in] T type of the binded attribute
     *
     * @param[in] name the name of the corresponding XML attribute
     * @param[in] binder Binder object containing mutator and accessor methods for binded attribute
     *                   @see makeBinder
     */
    template<class T>
    Attribute(const std::string &name, Binder<T> &&binder) :
        Attribute(name, Type<T>{}, Getter<T>(binder.first), Setter<T>(binder.second))
    {
    }


    virtual ~Attribute() = default;

    /** Retrieve the attribute name
     *
     * @return a string containing the name of the attribute
     */
    virtual const std::string &getName() const
    {
        return mAttributeImpl->getName();
    }

    /** Retrieve the raw attribute value
     *
     * @return a string the raw attribute value
     */
    virtual std::string get() const
    {
        return mAttributeImpl->get();
    }

    /** Set the raw attribute value
     *
     * @param[in] value the raw attribute value to set
     */
    virtual void set(const std::string& value)
    {
        mAttributeImpl->set(value);
    }

protected:

    /** Default constructor needed by implementenation subclass */
    Attribute() = default;

private:

    /** Implementation older
     *
     * shared_ptr is required because unique_ptr can't be copied.
     * As Attribute are going to be in lists, there will be a copy
     * when initializing through an initializer_list (which always copy)
     */
    const std::shared_ptr<Attribute> mAttributeImpl = nullptr;
};

namespace details
{

/** Effective Attribute implementation class
 * Provide a real handling of a particular type of Attribute
 *
 * @tparam the type of the data contained in the attribute
 */
template <typename T>
class AttributeImpl : public Attribute
{
public:

    /** @see Attribute */
    AttributeImpl(const std::string &name, Getter<T> getter, Setter<T> setter) :
        mName(name), mGetter(getter), mSetter(setter)
    {
    }

    virtual const std::string &getName() const override final
    {
        return mName;
    }

    virtual std::string get() const override final
    {
        return std::to_string(mGetter());
    }

    virtual void set(const std::string &rawValue) override final
    {
        T value;
        if (!convertTo(rawValue, value))
        {
            throw std::invalid_argument("Invalid type conversion during '" +
                                        mName + "' attribute set");
        }
        mSetter(value);
    }

private:

    /** XML attribute name */
    const std::string mName;

    /** Accessor of the binded variable attribute */
    const Getter<T> mGetter;

    /** Mutator of the binded variable attribute */
    const Setter<T> mSetter;
};

template <>
void AttributeImpl<std::string>::set(const std::string &rawValue)
{
    mSetter(rawValue);
}

template <>
std::string AttributeImpl<std::string>::get() const
{
    return mGetter();
}

template <>
std::string AttributeImpl<bool>::get() const
{
    return mGetter() ? "true" : "false";
}

} /** details namespace */

/** Renaming commonly used list of attributes types */
using Attributes = std::list<Attribute>;

} /** binding namespace */
} /** xml namespace */
} /** core namespace */
