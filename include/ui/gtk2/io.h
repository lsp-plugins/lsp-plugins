/*
 * io.h
 *
 *  Created on: 20 июля 2016 г.
 *      Author: sadko
 */

#ifndef UI_GTK2_IO_H_
#define UI_GTK2_IO_H_

namespace lsp
{
    /** Decode mouse controller buttons state to internal presentation
     *
     * @param flags GTK2 button state flags
     * @return encoded into mcf_t constants state flags
     */
    size_t gtk2_decode_mcf(size_t flags);

    /** Encode mouse controller buttons to internal presentation
     *
     * @param flags mcf_t constants state flags
     * @return encoded into GTK2 button state flags
     */
    size_t gtk2_encode_mcf(size_t flags);

    /** Decode mouse controller button to internal presentation
     *
     * @param buttons GTK2 button
     * @return encoded according to mcb_t constants button
     */
    size_t gtk2_decode_mcb(size_t buttons);

    /** Encode mouse controller button to GTK2 presentation
     *
     * @param buttons mcb_t button
     * @return encoded into GTK2 button code
     */
    size_t gtk2_encode_mcb(size_t buttons);

    /** Decode mouse direction button to internal presentation
     *
     * @param direction GTK2 scroll presentation
     * @return encoded according to mcd_t constants button
     */
    size_t gtk2_decode_mcd(size_t direction);

    /** Encode mouse direction button to GTK2 presentation
     *
     * @param direction mcd_t direction
     * @return encoded into GTK2 direction code
     */
    size_t gtk2_encode_mcd(size_t direction);
}

#endif /* UI_GTK2_IO_H_ */
