#ifndef enumlut_h
#define enumlut_h

// Dependencies for the logic inside of macros
#include <stddef.h>
#include <string.h>

// Generate enum and string enum entries
#define GENERATE_ENUM(ENUM, OFFS) ENUM = OFFS,
#define GENERATE_STRING(ENUM, OFFS) [OFFS] = #ENUM,

// TODO: Think about how to iterate enum values and provide a count of all entries by a getter function

typedef enum enumlut_result
{
  ENUMLUT_SUCCESS,
  ENUMLUT_NOT_FOUND
} enumlut_result_t;

/**
 * @brief Enum name getter function declaration
 * 
 * @param namearr Name of the array LUT that corresponds numeric
 * values to enum name strings
 * @param enum_name Name of the enum
 */
#define ENUM_GET_NAME_DECL(namearr, enum_name)                    \
  const char *namearr##_name(enum_name##_t key)

/**
 * @brief Enum name getter function implementation
 * 
 * @param namearr Name of the array LUT that corresponds numeric
 * values to enum name strings
 * @param enum_name Name of the enum
 */
#define ENUM_GET_NAME_IMPL(namearr, enum_name)                    \
  ENUM_GET_NAME_DECL(namearr, enum_name) {                        \
    /* Calculate the length of the array */                       \
    size_t length = sizeof(namearr) / sizeof(const char *);       \
    /* Key out of range */                                        \
    if (key < 0 || key >= length) return NULL;                    \
    return namearr[key];                                          \
  }

/**
 * @brief Enum value getter function declaration
 * 
 * @param namearr Name of the array LUT that corresponds numeric
 * values to enum name strings
 * @param enum_name Name of the enum
 */
#define ENUM_GET_VALUE_DECL(namearr, enum_name)                   \
  enumlut_result_t namearr##_value(const char *name, enum_name##_t *out)

/**
 * @brief Enum value getter function implementation
 * 
 * @param namearr Name of the array LUT that corresponds numeric
 * values to enum name strings
 * @param enum_name Name of the enum
 */
#define ENUM_GET_VALUE_IMPL(namearr, enum_name)                   \
  ENUM_GET_VALUE_DECL(namearr, enum_name) {                       \
    /* No name provided */                                        \
    if (!name) return ENUMLUT_NOT_FOUND;                          \
    /* Calculate the length of the array */                       \
    size_t length = sizeof(namearr) / sizeof(const char *);       \
    /* Search through all keys */                                 \
    for (size_t i = 0; i < length; i++)                           \
    {                                                             \
      /* Compare and return if the name matched */                \
      const char *curr = namearr[i];                              \
      if (curr && strcasecmp(name, curr) == 0)                    \
      {                                                           \
        *out = i;                                                 \
        return ENUMLUT_SUCCESS;                                   \
      }                                                           \
    }                                                             \
    /* Key not found */                                           \
    return ENUMLUT_NOT_FOUND;                                     \
  }

/**
 * @brief Generate the implementation for an enum lookup table
 * 
 * @param enum_type Typedef symbol of the enum
 * @param declfunc Makro function list that declares the values
 */
#define ENUM_LUT_IMPL(enum_name, declfunc)                        \
  static const char *enum_name[] = {                              \
    declfunc(GENERATE_STRING)                                     \
  };

/**
 * @brief Generate the enum typedefinition
 * 
 * @param enum_type Typedef symbol of the enum
 * @param declfunc Makro function list that declares the values
 */
#define ENUM_TYPDEF_IMPL(enum_name, declfunc)                     \
  typedef enum enum_name                                          \
  {                                                               \
    declfunc(GENERATE_ENUM)                                       \
  } enum_name##_t;

/**
 * @brief Generate the full LUT implementation, including LUT as
 * well as the name and value getter functions
 * 
 * INFO: Call this in the C file
 * 
 * @param enum_type Typedef symbol of the enum
 * @param declfunc Makro function list that declares the values
 */
#define ENUM_LUT_FULL_IMPL(enum_name, declfunc)                   \
  ENUM_LUT_IMPL(enum_name, declfunc);                             \
  ENUM_GET_NAME_IMPL(enum_name, enum_name);                       \
  ENUM_GET_VALUE_IMPL(enum_name, enum_name);

/**
 * @brief Generate the full typedef implementation, including typedefinition
 * as well as the name and value getter function declarations
 * 
 * INFO: Call this in the header file
 * 
 * @param enum_type Typedef symbol of the enum
 * @param declfunc Makro function list that declares the values
 */
#define ENUM_TYPEDEF_FULL_IMPL(enum_name, declfunc)               \
  ENUM_TYPDEF_IMPL(enum_name, declfunc);                          \
  ENUM_GET_NAME_DECL(enum_name, enum_name);                       \
  ENUM_GET_VALUE_DECL(enum_name, enum_name);

#endif