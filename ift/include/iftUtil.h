/**
 * @file iftUtil.h
 * @brief Structs and function prototypes for several Utilities, such that file management, strings, etc.
 *
 * An example of file management is found in demo/Miscelaneuous/iftPathnames.c
 *
 * @author Samuel Martins
 */

#ifndef _IFT_UTIL_H_
#define _IFT_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iftCommon.h"
#include "iftImage.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#if defined(__WIN32) || defined(__WIN64)
#include <Windows.h>
#endif


#define COLOR_BLUE "\x1B[34m"
#define COLOR_RESET "\033[0m"

// DIRECTORY SEPARATOR STRING

#if defined(__WIN32) || defined(__WIN64)
    #define SEP_C "\\"
#else
    #define SEP_C "/"
#endif

/** @addtogroup Utilities
 * @{ */

/** @addtogroup File
 * @brief File and path management functions.
 * @{ */

/**
 * @brief An abstract representation of file.
 *
 * \ref iftFile contains the file info for creating, using and managing disk files.
 *
 * @author Samuel Botter
 */
typedef struct _ift_file {
    /** File path */
    char pathname[512];
    /* shows if the file is stored on disk */
    char exists;
} iftFile;


/**
 * @brief An abstract representation of directories.
 *
 * Contains a list of files and subdirectories under that directory.
 *
 * @author Samuel Botter
 *
 */
typedef struct _ift_dir {
    char pathname[512];
    /** Number of files from the directory */
    int nfiles;
    /** Vector of iftFiles with the childs */
    iftFile **files;
    /** Number of files from the directory */
    int nsubdirs;
    /** Vector of iftDir with the subdirs */
    struct _ift_dir **subdirs;
} iftDir;

/********************** Pathname Management ***********************/

/**
 * @brief Creates an iftFile.
 *
 * @author Samuel
 *
 * Creates an iftFile from a <pathname> on memory, BUT IT DOES NOT CREATE IT ON DISK.
 * If the <pathname> exists, the field <exists> is set to 1.
 * Otherwise, it is set to 0.
 * If the file exists and is a directory, an error will be raised.
 *
 * @param pathname The file to be read.
 * @return An iftFile with the <pathname>.
 */
iftFile *iftCreateFile(char *pathname);

/**
 * @brief Loads an iftFile with type = DIR_TYPE.
 *
 * @author Samuel Botter
 *
 * @date Aug 14, 15
 *
 * Loads a directory from a <pathname> on memory with type = DIR_TYPE.
 * THE DIRECTORY MUST EXIST.
 * The parameter <hier_levels> indicates until which hierarchical level will be considered in the
 * loading.
 *
 * If hier_levels=2, all files and subdirs until the 2º level from the root directory are loaded, but
 * the content of the subdirs from the 2º level are not loaded.
 * If  hier_levels=0, all files and subdirs from all levels are loaded.
 * If <hier_levels> is greater than the real number of levels from the root directory, all files and subdirs
 * will be loaded.
 *
 * A separator char '/' (linux) or '\' (windows) is put at the end of the pathname.
 * If the file exists and is not a directory (it is a file), an error will be raised.
 *
 * @param dir_pathname The directory to be read.
 * @param hier_levels Number of levels which will be loaded.
 * @return An iftDir with the directory.
 */
iftDir *iftLoadDir(char *dir_pathname, int hier_levels);

/**
 * @brief Loads all files (not subdirs) in the directory <dir_pathname> from a given <extension>.
 *
 * @author Samuel
 *
 * Loads all files (not subdirs) in the directory <dir_pathname> from a given <extension>.
 * The filenames are sorted in ascending order.
 * It only gets the files from the 1st file hierarchical level.
 * If <extension> = "", it gets all files (not subdirs).
 *
 * @param dir_pathname The directory to be read.
 * @param extension Extension from the files.
 * @return An iftDir with all files with <extension> inside <dir_pathname>.
 */
iftDir *iftLoadFilesFromDirectory(char *dir_pathname, char *extension);

/**
 * @brief Destroys an iftFile.
 *
 * @author Samuel
 *
 * Destroys an iftFile from memory, BUT IT DOES NOT DELETE IT FROM THE DISK.
 *
 * @param f The iftFile to be destroyed.
 */
void iftDestroyFile(iftFile **file);

/**
 * @brief Destroys an iftDir.
 *
 * @author Samuel
 *
 * Destroys the iftDir <dir> from memory, BUT IT DOES NOT DELETE IT FROM THE DISK.
 * If its sub-directories also were listed/loaded, the function also destroys them.
 *
 * @param dir The iftDir to be destroyed.
 */
void iftDestroyDir(iftDir **dir);

/**
* @brief Prints the informations of an iftFile.
 *
 * @author Samuel
*
* Prints the informations of an iftFile.
* It does not print the information of sub-directories from an iftFile dir.
*
* @param f The iftFile to be printed.
 */
void iftPrintFileInfo(iftFile *f);

/**
* @brief Prints the informations of an iftDir.
 *
 * @author Samuel
*
* Prints the informations of an iftDir.
* It does not print the informations of its sub-directories.
*
* @param dir The iftDir to be printed.
 */
void iftPrintDirInfo(iftDir *dir);

/**
* @brief Prints all files and sub-directories from an iftFile directory as a tree.
 *
 * @author Samuel
*
* @param dir The iftDir to be printed.
 */
void iftPrintDirAsTree(iftDir *dir);

/**
* @brief Checks if the <pathname> is a file (not dir) and exists on the disk.
 *
 * @author Samuel
*
* @param pathname The pathname to be checked.
* @return 1 if it is file, 0 otherwise.
 */
int iftFileExists(char *pathname);

/**
* @brief Checks if the iftFile exists on the disk.
 *
 * @author Samuel
*
* @param f The iftFile to be checked.
* @return 1 if it is directory, 0 otherwise.
 */
int iftFileExists2(iftFile *f);

/**
* @brief Checks if the <pathname> is a directory on the disk.
 *
 * @author Samuel
*
* This function checks if the <pathname> is a directory on the disk.
*
* @param pathname The pathname to be checked.
* @return 1 if it is directory, 0 otherwise.
 */
int iftDirectoryExists(char *pathname);

/**
 * @brief Joins two pathnames.
 *
 * @author Samuel
 *
 * Returns the join of the pathname1 and pathname2.
 * It automatically treats the '/'' of the end of pathname1 and the begining of the pathname2.
 *
 * @param pathname1 The leftmost pathname to be joined.
 * @param pathname2 The rightmost pathname to be joined.
 * @return The joined pathname.
 */
char *iftJoinPathnames(char *pathname1, char *pathname2);

/**
* @brief Gets the parent dir from a file or directory.
 *
 * @author Samuel
*
* It does not check if the parent dir exists.
* Return the parent dir WITHOUT THE SLASH AT THE END: e.g: /home/samuel --> parent_dir = /home
*
* @param pathname The pathname of the file/directory.
* @return The parent directory from <pathname>.
 */
char *iftGetParentDir(char *pathname);

/**
* @brief Gets the absolute path from a pathname.
 *
 * @author Samuel
*
* @param pathname The pathname.
* @return The absolute pathname from <pathname>.
 */
char *iftAbsPathname(char *pathname);

/**
* @brief Gets the basename from a pathname.
 *
 * @author Samuel
*
* @param pathname The pathname.
* @return The basename from <pathname>.
 */
char *iftBasename(char *pathname);


/**
 * @brief Creates a directory on the disk.
 *
 * @author Samuel
 *
 * Creates the directory <pathname> directory on the disk.
 * If the directory already exists, do nothing.
 *
 * http://www.thegeekstuff.com/2012/06/c-directory/
 *
 * @param pathname The pathname of the directory to be created on disk.
 */
void iftMakeDir(char *pathname);

/**
 * @brief Creates a directory on the disk.
 *
 * @author Samuel
 *
 * Creates the directory <pathname> directory on the disk.
 * If the directory already exists, do nothing.
 *
 * @param pathname The pathname of the directory to be created on disk.
 * @return The iftDir of the directory created.
 */
iftDir *iftMakeDir2(char *pathname);


char *iftMakeTempFile();

//TODO Fix the functions below
// char *iftMakeTempPathname();
// char *iftMakeTempDir();
// iftDir *iftMakeTempDir2();

/**
 * @brief Remove a directory from the disk.
 *
 *  @author Samuel
 *
 * Remove the directory with <pathname> from the DISK.
 */
void iftRemoveDir(char *pathname);


/**
 * @brief Check if a file is empty.
 *
 *  @author Samuel
 *
 * @param fp File pointer to the file.
 * @return 1 if it is directory, 0 otherwise.
 */
int iftIsFileEmpty(FILE *fp);


/**
 * @brief Reads a FILE as a String.
 * 
 * @author Samuel
 *
 * Returns the content of a FILE with pathname @pathname in a string.
 * All the content of the file is stored in an only string.
 * 
 * @param pathname Pathname from the FILE to be read. 
 * @return The FILE content as a String.
 */
char *iftReadFileAsString(char *pathname);


/**
 * @brief Get the file index, according to the path.
 *
 * @author Peixinho
 *
 * The image index is stored as part of the image file name.
 * The IFT default nomenclature is <label>_<id>.<ext>
 *
 * Ex.: 000002_000015.pgm
 *
 * @param path Image path.
 * @return The image index.
 */
int iftImageIdx(char* path);

/**
 * @brief Get the file label, according to the path.
 *
 * @author Peixinho
 *
 * The image label is stored as part of the image file name.
 * The IFT default nomenclature is <label>_<id>.<ext>
 *
 * Ex.: 000002_000015.pgm
 *
 * @param path Image path.
 * @return The image label.
 */
int iftImageLabel(char* path);

/**
 * @brief Get the file labels, according to the filed path under that directory.
 *
 * @author Peixinho
 *
 * The image label is stored as part of the image file name.
 * The IFT default nomenclature is <label>_<id>.<ext>
 *
 * Ex.: 000002_000015.pgm
 *
 * @param dir Directory containing the files.
 * @return The image labels.
 */
int* iftImageLabels(iftDir *dir);

/**
 * @brief Load a batch of images indicated in @ref dir.
 *
 * @author Peixinho
 *
 * @param dir Directory containing the image files.
 * @return The images list.
 */
iftImage** iftLoadImages(iftDir* dir);

/** @} */

/*****************************************************************/

/**@addtogroup string
 * @brief String manipulation functions.
 * @{ */

/**
 * @brief Check if the string ends with a given substring.
 * @param str Search string.
 * @param suffix End substring.
 */
int iftEndswith(char *str, char *suffix);

/**
 * @brief Split a string into pieces according to a substring separator and returns the specified position (Also accepts negative indexing).
 *
 * @param phrase String to be splitted
 * @param delimiter Substring separator
 * @param i Index in the splitted string.
 *
 * @return The ith splitted string.
 */
char *iftSmartSplitString(char *phrase, char *delimiter, int i);

/** @} */

/** @} */


#ifdef __cplusplus
}
#endif

#endif
