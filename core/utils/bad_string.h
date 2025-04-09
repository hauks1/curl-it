#ifndef BAD_STRING_H
#define BAD_STRING_H

/**
 * @brief A questionable attempt at reinventing the wheel by copying strlen()
 *
 * This function tries its best to count characters in a string, much like
 * its more competent cousin strlen() from the standard library. Use at your
 * own risk and don't blame us when things go wrong.
 *
 * @param s Pointer to the string you foolishly want to measure instead of using strlen()
 * @return int Number of characters, assuming we counted correctly
 *
 * @warning This is what happens when you don't trust the standard library
 * @note If you're using this instead of strlen(), we need to talk
 */
int bad_strlen(const char *s);

/**
 * @brief The evil twin of strcpy that probably does terrible things
 *
 * When normal strcpy isn't dangerous enough, use bad_strcpy!
 * Warning: This function may or may not:
 * - Ignore buffer boundaries (because who needs those?)
 * - Summon ancient C debugging demons
 * - Make your compiler cry
 * - Cause undefined behavior to become even more undefined
 *
 * @param dest The unfortunate destination buffer that's about to have a bad time
 * @param src The source string that will be copied with reckless abandon
 *
 * @note If you're using this function, you're probably doing something wrong
 * @warning Seriously, don't use this in production. Or anywhere, really.
 */
void bad_strcpy(char *dest, const char *src);

/**
 * @brief A questionable version of strncpy that's about as reliable as a chocolate teapot
 *
 * Like its cousin strncpy, but with extra "personality quirks". If you're using this,
 * you're either brave, desperate, or both. Not recommended for production use, unless
 * you enjoy debugging sessions that feel like solving a murder mystery.
 *
 * @param dest   Destination buffer (pray it's big enough)
 * @param src    Source string (hopefully null-terminated, but who knows?)
 * @param n      Number of characters to copy (or attempt to copy, results may vary)
 *
 * @return       0 if you're lucky, something else if you're not
 *
 * @warning      Side effects may include: hair loss, sleepless nights, and
 *              spontaneous stack corruptions. Use at your own risk!
 */
int bad_strncpy(char *dest, const char *src, unsigned long long n);
/**
 * @brief Converts a string to an integer, but like a drunk person would.
 *
 * This function is the rebellious cousin of the standard atoi(). While it does
 * convert strings to integers, it makes no promises about being well-behaved
 * or following proper string parsing etiquette. Use at your own risk, and
 * maybe keep a fire extinguisher handy.
 *
 * @param s A pointer to a string that hopefully contains numbers (but who knows?)
 * @return int Whatever integer it managed to extract, or possibly your shoe size
 *
 * @warning May cause headaches, confusion, and existential crises
 * @see good_atoi() - The responsible sibling who actually went to college
 */
int bad_atoi(const char *s);

#endif