//
//  Value.h
//  hifem
//
//  Created by pshiraz on 2/4/14.
//  Copyright (c) 2014 pshiraz. All rights reserved.
//

#ifndef hifem_Value_h
#define hifem_Value_h


#include <typeinfo>
#include <stdexcept>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include "loki/TypeTraits.h"
#include "String.h"
#include "MathBase.h"
#include "Vec.h"

using namespace std;
using namespace Loki;
using namespace PS::MATH;

namespace PS {
    
    namespace PRIVATE {
        //Interface
        class placeholder {
        public:
            virtual ~placeholder() {}
            virtual const std::type_info& type_info() const = 0;
            virtual std::string toString() const = 0;
            virtual void fromString(const string& s) = 0;
            virtual placeholder* clone() const = 0;
        };
        
        //Internal Placeholder
        template <typename value_type>
        class holder : public placeholder {
        public:
            holder(const value_type& v):held(v) {
            }
            
            
            virtual const std::type_info& type_info() const {
                return typeid(value_type);
            }
            
            //To/From String
            virtual std::string toString() const {
                
                stringstream reader;
                reader << held;
                
                string result;
                reader >> result;
                return result;
            }
            
            virtual void fromString(const string& s) {

                TypeTraits<value_type> traits;

                if(traits.isStdIntegral) {
                    stringstream reader;
                    reader << s;
                    held = value_type();
                    reader >> held;
                }
                else
                    throw std::invalid_argument("This type cannot be converted to string. Extend Value.h file to support it!");
            }
            
            
            virtual placeholder* clone() const {
                return new holder(held);
            }
            
            value_type held;
        };

		//Partial Specialization for vec2f
		template<>
		class holder<AnsiStr> : public placeholder {
		public:
			holder(const AnsiStr& str) :
					held(str) {
			}

			std::string toString() const {
				char buffer[1024];
				sprintf(buffer, "%s", held.cptr());
				return string(buffer);
			}

			void fromString(const string& s) {
				held = AnsiStr(s.c_str());
			}

			const std::type_info& type_info() const {
				return typeid(AnsiStr);
			}

			placeholder* clone() const {
				return new holder(held);
			}

			AnsiStr held;
		};

        //Partial Specialization for vec2f
        template <>
        class holder<vec2f> : public placeholder {
        public:
            holder(const vec2f& v):held(v) {}
            
            std::string toString() const {
                char buffer[1024];
                sprintf(buffer, "(%f, %f)", held.x, held.y);
                return string(buffer);
            }
            
            void fromString(const string& s) {
                float f1, f2;
                sscanf(s.c_str(), "(%f, %f)", &f1, &f2);
                held = vec2f(f1, f2);
            }
            
            const std::type_info& type_info() const {
                return typeid(vec2f);
            }
            
            placeholder* clone() const {
                return new holder(held);
            }
            
            vec2f held;
        };
        
        //Partial Specialization for vec3f
        template <>
        class holder<vec3f> : public placeholder {
        public:
            holder(const vec3f& v):held(v) {}
            
            std::string toString() const {
                char buffer[1024];
                sprintf(buffer, "(%f, %f, %f)", held.x, held.y, held.z);
                return string(buffer);
            }
            
            void fromString(const string& s) {
                float f1, f2, f3;
                sscanf(s.c_str(), "(%f, %f, %f)", &f1, &f2, &f3);
                held = vec3f(f1, f2, f3);
            }
            
            const std::type_info& type_info() const {
                return typeid(vec3f);
            }
            
            placeholder* clone() const {
                return new holder(held);
            }
            
            vec3f held;
        };
        
        //Partial Specialization for vec4f
        template <>
        class holder<vec4f> : public placeholder {
        public:
            holder(const vec4f& v):held(v) {}
            
            std::string toString() const {
                char buffer[1024];
                sprintf(buffer, "(%f, %f, %f, %f)", held.x, held.y, held.z, held.w);
                return string(buffer);
            }
            
            void fromString(const string& s) {
                float f1, f2, f3, f4;
                sscanf(s.c_str(), "(%f, %f, %f, %f)", &f1, &f2, &f3, &f4);
                held = vec4f(f1, f2, f3, f4);
            }
            
            const std::type_info& type_info() const {
                return typeid(vec3f);
            }
            
            placeholder* clone() const {
                return new holder(held);
            }
            
            vec4f held;
        };

    }

    ///////////////////////////////////////////////////////
    class Value {
    public:
        Value():content(NULL) {
            
        }

        //Copy Constructor semantics
        template <typename value_type>
        Value(const value_type& v) : content(new PRIVATE::holder<value_type>(v)) {
            
        }
        
        Value(const Value& other):content(other.content ? other.content->clone() : 0) {
            
        }
        
        ~Value() {
            SAFE_DELETE(content);
        }
        
        
        const std::type_info& type_info() const {
            return (content ? content->type_info() : typeid(void));
        }

        //String IO
        const std::string toString() const {
            return (content ? content->toString() : string("NA"));
        }
        
        void fromString(const string& s) {
            if(content)
                content->fromString(s);
        }
        
        //Get Value
        template <typename value_type>
        value_type get() const {
            return (typeid(value_type) == type_info()) ?
            static_cast< PRIVATE::holder<value_type> *>(content)->held : throw std::bad_cast();
        }
        
        template <typename value_type>
        const value_type* cptr() const {
            return (typeid(value_type) == type_info()) ? &static_cast< PRIVATE::holder <value_type> *>(content)->held : 0;
            
        }
        
        Value& swap(Value& rhs) {
            std::swap(content, rhs.content);
            return (*this);
        }
        
        
        //Operators
        Value& operator=(const Value& rhs) {
            Value tmp(rhs);
            return swap(tmp);
        }

        /*
        Value& operator=(const string& rhs) {
            Value tmp(rhs);
            return swap(tmp);
        }
         */

        template<typename value_type>
        Value& operator=(const value_type& rhs) {
            Value tmp(rhs);
            return swap(tmp);
        }
        
        operator const void*() const {
            return content;
        }
    private:
        
        
        PRIVATE::placeholder* content;
    };
    
    template <typename value_type>
    value_type value_cast(const Value& operand) {
        const value_type* result = operand.cptr<value_type>();
        return result ? *result : throw::bad_cast();
    }
}

#endif
