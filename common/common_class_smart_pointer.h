#ifndef COMMON_CLASS_SMART_POINTER_H
#define COMMON_CLASS_SMART_POINTER_H

#include "common_class_base.h"
#include "common_class_company_profile.h"
#include "common_class_set.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class

template <typename T>
class SmartPointer
{
private:
	bool need_release;
	T* data_ptr;

public:
	SmartPointer();
	SmartPointer(T* ptr);
	~SmartPointer();
	T& operator*();
	const T& operator*() const;
	T* operator->();
	const T* operator->() const;
	void set_new(T* ptr);
	T* get_instance();
	const T* get_const_instance()const;
	void disable_release();
};
//typedef SmartPointer SP;

#endif
