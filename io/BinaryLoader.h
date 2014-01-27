/* 
 * File:   BinaryLoader.h
 * Author: meckya
 *
 * Created on 9. Januar 2014, 09:51
 */

#ifndef BINARYLOADER_H
#define	BINARYLOADER_H
#include "BinaryWriter.h"

class BinaryLoader {
    
public:
    BinaryLoader();
    BinaryLoader(const BinaryLoader& orig);
    virtual ~BinaryLoader();
    /*
     * Opens a file for processing
     * 
     * @paramter name - name of the file
     * @parameter signatur - pointer to a list of ints representing a signature
     * @parameter signatureLength - number of signature entries
     * @return IO_OK in case of success or error code     
     */
    uint32 open(const char* name,int* signature,uint32 signatureLength);
    /*
     * Opens the current chunk and reads the chunk header     
     */
    uint32 openChunk();
    /*
     * Closes the current chunk and moves the file pointer to the next
     * available chunk
     */
    void closeChunk();
    /*
     * Returns the ID of the current chunk header
     * 
     * @return current ID
     */    
    const uint32 getChunkID() const {
        return m_CurrentHeader->id;
    }
    /*
     * Returns the version of the current chunk
     * 
     * @return current version
     */
    const uint32 getChunkVersion() const {
        return m_CurrentHeader->version;
    }
    /*
     * Closes the file
     * 
     * @return IO_OK in case of success or an error code
     */
    uint32 close();
    /*
     * Returns the current error code 
     * 
     * @return current error code
     */
    const uint32 getStatusCode() const {
        return m_ErrorCode;
    }
    void read(float* value);
    void read(int* value);
    /*
     * Reads data into buffer
     * 
     * buffer - the buffer that will be filled
     * size - the number of bytes to read
     * 
     * Returns: IO_OK if successfull or any error code
     */
    uint32 read(void* buffer,uint32 size);
    /*
     * Reset the internal file pointer to the beginning
     */
    void reset();
private:
    uint32 m_StartPos;
    ChunkHeader* m_CurrentHeader;
    int m_CurrentPos;
    FILE* m_Stream;
    uint32 m_ErrorCode;
    uint32 m_Index;
};

#endif	/* BINARYLOADER_H */

