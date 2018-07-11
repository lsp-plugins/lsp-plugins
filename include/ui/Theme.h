/*
 * Theme.h
 *
 *  Created on: 09 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef UI_THEME_H_
#define UI_THEME_H_

namespace lsp
{
    
    class Theme
    {
        private:
            typedef struct color_data_t
            {
                color_data_t *next;
                char *name;
                Color color;
            } color_data_t;

        private:
            color_data_t    *pColors;

        private:
            bool findColor(const char *name, Color *dst);
            static ssize_t readComponent(const char *text, size_t digits);
            static bool getComponents(const char *text, float &c1, float &c2, float &c3);
            bool parseRGBColor(const char *text, Color *dst);
            bool parseHSLColor(const char *text, Color *dst);

        public:
            inline Theme()
            {
                pColors     = NULL;
            }

            ~Theme();

            /** Load theme from XML
             *
             * @param path path to the file
             * @return true if theme was loaded
             */
            bool load(const char *path);

            /** Add color to theme
             *
             * @param name
             * @param value
             * @return
             */
            bool addColor(const char *name, const char *value);

            /** Add color to theme
             *
             * @param name color name
             * @param color color value
             * @return true if added
             */
            bool addColor(const char *name, const Color *color);

            /** Add color to theme
             *
             * @param name color name
             * @param color color value
             * @return true if added
             */
            inline bool addColor(const char *name, const Color &color)
            {
                return addColor(name, &color);
            }

            /** Get color by it's symbolic name
             *
             * @param name color name
             * @param dst pointer to store object
             * @return false if default color was used
             */
            bool getColor(const char *name, Color *dst);

            /** Get color by it's symbolic name
             *
             * @param name color name
             * @param dst pointer to store object
             * @return false if default color was used
             */
            inline bool getColor(const char *name, Color &dst)
            {
                return getColor(name, &dst);
            }

            /** Get color by it's symbolic name
             *
             * @param color color ID
             * @param dst pointer to store object
             * @return false if default color was used
             */
            inline bool getColor(color_t color, Color *dst)
            {
                return getColor(color_name(color), dst);
            }

            /** Get color by it's symbolic name
             *
             * @param color color ID
             * @param dst pointer to store object
             * @return false if default color was used
             */
            inline bool getColor(color_t color, Color &dst)
            {
                return getColor(color_name(color), &dst);
            }
    };

} /* namespace lsp */

#endif /* UI_THEME_H_ */
