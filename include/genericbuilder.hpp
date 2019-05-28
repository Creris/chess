#pragma once

#ifndef GENERIC_BUILDER_HEADER_H_
#define GENERIC_BUILDER_HEADER_H_

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <type_traits>

/*
	This file contains a definition for:
		- BuilderFunction type declaration.
		- _all_convertible and _all_convertible_v, meant for internal use only.
		- Generic templated Builder class.
*/


/**
	A type that the function that is registered into a builder returns.

	\sa Builder()
*/
template <class Base>
using BuilderFunction = std::shared_ptr<Base>(*)();

/**
	A meta class that derives from integral_constant and its member value
	is equal to true, if all types First and Rest... are convertible into Test,
	and false otherwise.
*/
template <class Test, class First, class... Rest>
class _all_convertible;

template <class Test, class Other>
class _all_convertible<Test, Other> : public std::integral_constant<bool, std::is_convertible_v<Other, Test>> {
};

template <class Test, class First, class... Rest>
class _all_convertible : public std::integral_constant<bool, std::is_convertible_v<First, Test>&&
	_all_convertible<Test, Rest...>::value> {
};

/**
	A shortcut definition for _all_convertible::value.

	\sa _all_convertible()
*/
template <class Test, class... Rest>
constexpr auto _all_convertible_v = _all_convertible<Test, Rest...>::value;

template <class Builds>
class Builder {
	/**
		A map of registered builders for given names.

		Accessible by integer index.
	*/
	std::unordered_map<uint32_t, BuilderFunction<Builds>> registered;

	/**
		A helper list of strings that translates the string to integer.

		This integer is used as access to retreive the builder function.
	*/
	std::vector<std::pair<std::string, uint32_t>> stringConverter;

	/**
		Appends the singular value into stringConverter.

		\param idx The second value of the pair appended into stringConverter.
		\param f The first value of the pair appended into stringConverter.
	*/
	template <class First>
	void _append(uint32_t idx, First&& f) {
		stringConverter.emplace_back(std::forward<First>(f), idx);
	}

	/**
		Appends a list of values into stringConverter.

		\param idx The second value of all pairs appended into stringConverter.
		\param f One of the first values of the pair appended into stringConverter.
		\param ts Remaining list of first values of the pair appended into stringConverter.
	*/
	template <class First, class... T>
	void _append(uint32_t idx, First&& f, T&& ... ts) {
		stringConverter.emplace_back(std::forward<First>(f), idx);
		_append<T...>(idx, std::forward<T>(ts)...);
	}

	/**
		Converts the string into a index that can be used to directly
		access the map.
	*/
	inline uint32_t _toIdx(const std::string& s) const {
		for (auto& pair : stringConverter)
			if (pair.first == s)	return pair.second;
		return -1;
	}

	//Lock construction
	Builder() {}
	~Builder() = default;
	Builder(const Builder&) = delete;
	Builder(Builder&&) noexcept = delete;
	Builder& operator=(const Builder&) = delete;
	Builder& operator=(Builder&&) noexcept = delete;

public:
	/**
		Registers a function as a builder for given list of strings.

		Also registers an integer that can be used to directly access this
		builder function.

		\param builder A function that will be called when building new
			   instance. Has to take nothing and return BuilderFunction<Base>.
		\param strings List of strings that should all call this function.
		\return Registered index to directly access the function.
	*/
	template <class Func, class... T,
		class SFNIAE = std::enable_if_t<_all_convertible_v<std::string, T...>>
	>
	uint32_t registerBuilder(Func builder, T&& ... strings) {
		uint32_t idx = stringConverter.size();
		_append(idx, std::forward<T>(strings)...);

		registered.insert_or_assign(idx, static_cast<BuilderFunction<Builds>>(builder));
		return idx;
	}

	/**
		Returns an integer that can be used to access the builder that will be
		accessed if the string were used instead.

		\param s Access string.
	*/
	uint32_t nameToIndex(const std::string& s) const {
		return _toIdx(s);
	}

	/**
		Returns all integers that have unique builders registered to them.
		
		\return Vector of all integers that can be used to access the functions.
	*/
	std::vector<uint32_t> getAllOptions() const {
		std::vector<uint32_t> v;

		auto _isIn = [](const std::vector<uint32_t>& storage, uint32_t value) {
			return std::find(storage.begin(), storage.end(), value) != storage.end();
		};

		for (size_t i = 0; i < stringConverter.size(); ++i) {
			auto& val = stringConverter[i];
			if (!_isIn(v, val.second)) {
				v.emplace_back(val.second);
			}
		}

		return v;
	}

	/**
		Returns all strings that have builders attached to them.
		
		If 

		\param duplicates When false, returns only one string per builder function,
			   otherwise returns all strings.

		\return Vector of all strings according to the parameter.
	*/
	std::vector<std::string> getAllOptions(bool duplicates) const {
		std::vector<std::string> v;

		if (duplicates) {
			v.resize(stringConverter.size());
			std::transform(stringConverter.begin(), stringConverter.end(), v.begin(),
				[](const std::pair<std::string, uint32_t>& p) {
					return p.first;
				}
			);
		}
		else {
			for (size_t i = 0; i < stringConverter.size(); ++i) {
				v.emplace_back(stringConverter[i].first);
				auto idx = stringConverter[i].second;
				while (i < stringConverter.size() - 1
					&& stringConverter[i + 1].second == idx)	++i;
			}
		}

		v.shrink_to_fit();
		return v;
	}

	/**
		Get a builder function from index.

		\param idx With index. Can be obtained from nameToIndex function,
			   getAllOptions without parameters or from register function.
		\return Builder function that builds values for this index. Returns
				nullptr if such builder doesn't exist.

		\sa nameToIndex()
		\sa getAllOptions()
		\sa registerBuilder()
	*/
	BuilderFunction<Builds> getFunction(uint32_t idx) const {
		auto iter = registered.find(idx);
		if (iter == registered.end())
			return nullptr;
		return iter->second;
	}

	/**
		Get a builder function from a string.

		\param str A string that was registered as an identifier of this builder
		           when calling registerBuilder.
		\return Builder function that builds values for this index. Returns
				nullptr if such builder doesn't exist.
	*/
	BuilderFunction<Builds> getFunction(const std::string& str) const {
		return getFunction(_toIdx(str));
	}
	
	/**
		Find if the builder has this index registered.

		\param idx Index to check for.
		\return True if this index can be used to build new instance, false otherwise.
	*/
	bool isAvailable(uint32_t idx) const {
		return registered.find(idx) != registered.end();
	}

	/**
		Find if the builder has this string registered.

		\param str String to check for.
		\return True if this string can be used to build new instance, false otherwise.
	*/
	bool isAvailable(const std::string& str) {
		return _toIdx(str) != -1;
	}

	/**
		Get and execute a builder for this index.

		Calls getFunction and then executes the function. If getFunction returned
		nullptr, returns nullptr.

		\param idx Index to execute the builder with.
		\return new shared_ptr instance, or nullptr, if no function was registered
				for this index.
	*/
	auto build(uint32_t idx) const {
		auto f = getFunction(idx);
		return f ? f() : nullptr;
	}

	/**
		Get and execute a builder for this string.

		Calls getFunction and then executes the function. If getFunction returned
		nullptr, returns nullptr.

		\param str String to execute the builder with.
		\return new shared_ptr instance, or nullptr, if no function was registered
				for this string.
	*/
	auto build(const std::string& str) const {
		auto f = getFunction(str);
		return f ? f() : nullptr;
	}

	/**
		Get a singleton instance of this builder type.

		\return A singleton of this type.
	*/
	static Builder& getInstance() {
		static Builder b;
		return b;
	}
};

/**
	Allows registration of building functions for given strings during compilation.

	This class can be instantiated in global scope for this behaviour to appear.
	
	There is no guarantee of any order of registration, so trying to build
	instances with the builder in global variable initialization results in
	undefined behaviour.
*/
template <class T>
class BuilderRegister {
public:
	/**
		Register the function for given list of strings to the builder of type T.
	*/
	template <class Func, class... Strings>
	BuilderRegister(Func f, Strings&&... strings) {
		Builder<T>::getInstance().registerBuilder(f, std::forward<Strings>(strings)...);
	}
};

#endif // GENERIC_BUILDER_HEADER_H_
