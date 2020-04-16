/*
 * data.h
 *
 *  Created on: 24 мая 2019 г.
 *      Author: sadko
 */

#ifndef CORE_PORT_DATA_H_
#define CORE_PORT_DATA_H_

#include <stdarg.h>

#include <core/types.h>
#include <core/status.h>
#include <core/protocol/osc.h>

namespace lsp
{
    enum mesh_state_t
    {
        M_WAIT,         // Mesh is waiting for data request
        M_EMPTY,        // Mesh is empty
        M_DATA          // Mesh contains data
    };

    // Mesh port structure
    typedef struct mesh_t
    {
        volatile mesh_state_t   nState;
        size_t                  nBuffers;
        size_t                  nItems;
        float                  *pvData[];

        inline bool isEmpty() const         { return nState == M_EMPTY; };
        inline bool containsData() const    { return nState == M_DATA; };
        inline bool isWaiting() const       { return nState == M_WAIT;  };

        inline void data(size_t bufs, size_t items)
        {
            nBuffers    = bufs;
            nItems      = items;
            nState      = M_DATA; // This should be the last operation
        }

        inline void cleanup()
        {
            nBuffers    = 0;
            nItems      = 0;
            nState      = M_EMPTY; // This should be the last operation
        }

        inline void markEmpty()
        {
            nState      = M_EMPTY; // This should be the last operation
        }

        inline void setWaiting()
        {
            nState      = M_WAIT; // This should be the last operation
        }
    } mesh_t;

    /**
     * This interface describes frame buffer. All data is stored as a single rolling frame.
     * The frame consists of M data rows, each row contains N floating-point numbers.
     * While frame buffer is changing, new rows become appended to the frame buffer. Number
     * of appended/modified rows is stored in additional counter to allow the UI apply
     * changes incrementally.
     */
    typedef struct frame_buffer_t
    {
        protected:
            size_t              nRows;              // Number of rows
            size_t              nCols;              // Number of columns
            uint32_t            nCapacity;          // Capacity (power of 2)
            volatile uint32_t   nRowID;             // Unique row identifier
            float              *vData;              // Aligned row data
            uint8_t            *pData;              // Allocated row data

        public:
            static frame_buffer_t  *create(size_t rows, size_t cols);
            static void             destroy(frame_buffer_t *buf);

            status_t                init(size_t rows, size_t cols);
            void                    destroy();

        public:
            /**
             * Return the actual data of the requested row
             * @param dst destination buffer to store result
             * @param row_id row number
             */
            void read_row(float *dst, size_t row_id) const;

            /**
             * Get pointer to row data of the corresponding row identifier
             * @param row_id unique row identifier
             * @return pointer to row data
             */
            float *get_row(size_t row_id) const;

            /**
             * Get pointer to row data of the current row identifier
             * @param row_id unique row identifier
             * @return pointer to data of the next row
             */
            float *next_row() const;

            /**
             * Return actual number of rows
             * @return actual number of rows
             */
            inline size_t rows() const { return nRows; }

            /**
             * Get number of next row identifier
             * @return next row identifier
             */
            inline uint32_t next_rowid() const { return nRowID; }

            /**
             * Return actual number of columns
             * @return actual number of columns
             */
            inline size_t cols() const { return nCols; }

            /**
             * Clear the buffer contents, set number of changes equal to buffer rows
             */
            void clear();

            /**
             * Seek to the specified row
             * @param row_id unique row identifier
             */
            void seek(uint32_t row_id);

            /** Append the new row to the beginning of frame buffer and increment current row number
             * @param row row data contents
             */
            void write_row(const float *row);

            /** Overwrite the row of frame buffer
             * @param row row data contents
             */
            void write_row(uint32_t row_id, const float *row);

            /**
             * Just increment row counter to commit row data
             */
            void write_row();

            /**
             * Synchronize data to the other frame buffer
             * @param fb frame buffer object
             * @return true if changes from other frame buffer have been applied
             */
            bool sync(const frame_buffer_t *fb);

    } frame_buffer_t;

    /**
     * Buffer to transfer OSC packets between two threads.
     * It is safe to use if one thread is reading data and one thread is
     * submitting data. Otherwise, additional synchronization mechanism
     * should be used
     */
    typedef struct osc_buffer_t
    {
        volatile size_t     nSize;
        size_t              nCapacity;
        size_t              nHead;
        size_t              nTail;
        uint8_t            *pBuffer;
        uint8_t            *pTempBuf;
        size_t              nTempSize;
        void               *pData;

        /**
         * Clear the buffer
         */
        void                clear();

        /**
         * Get buffer size
         * @return buffer size
         */
        inline size_t       size() const { return nSize; }

        /**
         * Initialize buffer
         * @param capacity the buffer capacity
         * @return status of operation
         */
        static osc_buffer_t *create(size_t capacity);

        /**
         * Destroy the buffer
         */
        static void destroy(osc_buffer_t *buf);

        /**
         * Reserve space for temporary buffer, by default 0x1000 bytes
         * @return status of operation
         */
        status_t    reserve(size_t size);

        /**
         * Submit OSC packet to the queue
         * @param data packet data
         * @param size size of the data
         * @return status of operation
         */
        status_t    submit(const void *data, size_t size);

        /**
         * Submit OSC packet to the queue
         * @param data packet data
         * @param size size of the data
         * @return status of operation
         */
        status_t    submit(const osc::packet_t *packet);

        status_t submit_int32(const char *address, int32_t value);
        status_t submit_float32(const char *address, float value);
        status_t submit_string(const char *address, const char *s);
        status_t submit_blob(const char *address, const void *data, size_t bytes);
        status_t submit_int64(const char *address, int64_t value);
        status_t submit_double64(const char *address, double value);
        status_t submit_time_tag(const char *address, uint64_t value);
        status_t submit_type(const char *address, const char *s);
        status_t submit_symbol(const char *address, const char *s);
        status_t submit_ascii(const char *address, char c);
        status_t submit_rgba(const char *address, const uint32_t rgba);
        status_t submit_midi(const char *address, const midi::event_t *event);
        status_t submit_midi_raw(const char *address, const void *event, size_t bytes);
        status_t submit_bool(const char *address, bool value);
        status_t submit_null(const char *address);
        status_t submit_inf(const char *address);

        /**
         * Try to send message
         * @param address message address
         * @param params message parameters
         * @param args list of arguments
         * @return status of operation
         */
        status_t    submit_message(const char *address, const char *params...);

        /**
         * Try to send message
         * @param ref forge reference
         * @param address message address
         * @param params message parameters
         * @param args list of arguments
         * @return status of operation
         */
        status_t    submit_messagev(const char *address, const char *params, va_list args);

        /**
         * Fetch OSC packet to the already allocated memory
         * @param data pointer to store the packet data
         * @param size pointer to store size of fetched data
         * @param limit
         * @return status of operation
         */
        status_t    fetch(void *data, size_t *size, size_t limit);

        /**
         * Fetch OSC packet to the already allocated memory
         * @param packet pointer to packet structure
         * @param limit maximum available size of data for the packet
         * @return status of operation
         */
        status_t    fetch(osc::packet_t *packet, size_t limit);

        /**
         * Skip current message in the buffer
         * @return number of bytes skipped
         */
        size_t      skip();
    } osc_buffer_t;

    // Path port structure
    typedef struct path_t
    {
        /** Virtual destructor
         *
         */
        virtual ~path_t();

        /** Initialize path
         *
         */
        virtual void init();

        /** Get actual path
         *
         * @return actual path
         */
        virtual const char *get_path();

        /**
         * Get current flags
         * @return current flags
         */
        virtual size_t get_flags();

        /** Check if there is pending request
         *
         * @return true if there is a pending state-change request
         */
        virtual bool pending();

        /** Accept the pending request for path change,
         * the port of the path will not trigger as changed
         * until commit() is called
         */
        virtual void accept();

        /** Check if there is accepted request
         *
         * @return true if there is accepted request
         */
        virtual bool accepted();

        /** The state change request was processed,
         * the port is ready to receive new events,
         * this method SHOULD be called ONLY AFTER
         * we don't need the value stored in this primitive
         *
         */
        virtual void commit();
    } path_t;

    // Position port structure
    typedef struct position_t
    {
        /** Current sample rate in Hz
         *
         */
        float           sampleRate;

        /** The rate of the progress of time as a fraction of normal speed.
         * For example, a rate of 0.0 is stopped, 1.0 is rolling at normal
         * speed, 0.5 is rolling at half speed, -1.0 is reverse, and so on.
         */
        double          speed;

        /** Frame number
         *
         */
        uint64_t        frame;

        /** Time signature numerator (e.g. 3 for 3/4)
         *
         */
        double          numerator;

        /** Time signature denominator (e.g. 4 for 3/4)
         *
         */
        double          denominator;

        /** Current tempo in beats per minute
         *
         */
        double          beatsPerMinute;

        /** Current tick within beat
         *
         */
        double          tick;

        /** Number of ticks per beat
         *
         */
        double          ticksPerBeat;

        static void init(position_t *pos);
    } position_t;
}

#endif /* CORE_PORT_DATA_H_ */
