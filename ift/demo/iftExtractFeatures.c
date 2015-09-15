#include "ift.h"
#include "iftExtractFeatures.h"

int main(int argc, char *argv[])
{
    iftImage            *gt_img, *cand_img, *bb_img, *orig_img;
    iftDir              *gt_dir, *orig_dir, *cand_dir;
    iftDataSet          *cand_dataset;
    iftFeatures         *feat;
    iftAdjRel           *A;
    int                 p, i, j, t, descriptor_size;
    int                 num_candidates, total_candidates, numpixels_gt, index_cand;
    int                 *overlap_count;
    FILE                *f;
    //timer               *t1=NULL,*t2=NULL;

    /*--------------------------------------------------------*/

    int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

    /*--------------------------------------------------------*/

    if (argc!=6){
        fprintf(stdout,"Usage: iftExtractFeatures <dir_gt_detection> <dir_candidates> <opf_dataset>\n");
        fprintf(stdout,"       dir_orig_images:        directory of original images\n");
        fprintf(stdout,"       dir_gt_detection:       directory of trainig images (binary images)\n");
        fprintf(stdout,"       dir_candidates:         directory of candidate images (gray-scale images)\n");
        fprintf(stdout,"       opf_dataset   :         dataset of candidates (positive and negative samnples)\n");
        fprintf(stdout,"       text_file     :         text file to indicate the image and candidate number\n");
        exit(1);
    }

    orig_dir = iftLoadFilesFromDirectory(argv[1], "pgm");
    gt_dir = iftLoadFilesFromDirectory(argv[2], "pgm");
    cand_dir = iftLoadFilesFromDirectory(argv[3], "pgm");
    descriptor_size = DESCRIPTOR_SIZE; // BIC descriptor size = 128
    A = iftCircular(1.5);

    // Obtain array of filenames
    f = fopen(argv[5], "w");

    total_candidates = 0;
    // Alloc the array to count overlapping pixels
    for (i = 0; i < gt_dir->nfiles ; ++i) {
        // Read candidate images
        cand_img = iftReadImageByExt(cand_dir->files[i]->pathname);
        num_candidates = iftMaximumValue(cand_img);
        total_candidates += num_candidates;
        iftDestroyImage(&cand_img);
    }
    cand_dataset = iftCreateDataSet(total_candidates, descriptor_size);
    cand_dataset->nclasses = 2;

    // Select candidates
    index_cand = 0;
    for (i = 0; i < gt_dir->nfiles ; i++) {
        // Read gt and candidate images
        gt_img = iftReadImageByExt(gt_dir->files[i]->pathname);
        cand_img = iftReadImageByExt(cand_dir->files[i]->pathname);
        orig_img = iftReadImageByExt(orig_dir->files[i]->pathname);
        numpixels_gt = 0;
        // Compute the overlap between the gt and the candidates
        num_candidates = iftMaximumValue(cand_img);
        printf("num_candidates : %d\n", num_candidates);
        overlap_count = iftAllocIntArray(num_candidates);
        // Count overlapping pixels
        for (p = 0; p < gt_img->n ; p++) {
            if (gt_img->val[p] != 0 && cand_img->val[p] != 0) {
                overlap_count[cand_img->val[p]-1]++;
                numpixels_gt++;
            }
        }
        // Select positive and negative examples
        for (j = 0; j < num_candidates ; j++) {
            // Get bounding box
            bb_img = iftCreateBoundingBox2D(orig_img, cand_img, (j+1));
            // Extract BIC descriptor
            feat = iftExtractBIC(bb_img, descriptor_size/2);
            for (t = 0; t < feat->n ; t++) {
                cand_dataset->sample[index_cand].feat[t] = feat->val[t];
            }

            // if the candidate matches with more than 50% of the gt
            if (overlap_count[j] > (numpixels_gt/2+1)) {
                cand_dataset->sample[index_cand].truelabel = 1; // positive
            } else {
                cand_dataset->sample[index_cand].truelabel = 2; // negative
            }

            // Write image name and candidate number
            fprintf(f,"%s %d\n", orig_dir->files[i]->pathname, (j+1));
            // Increment candidate index
            index_cand++;
            iftDestroyImage(&bb_img);
            iftDestroyFeatures(&feat);
        }
        // Free
        iftDestroyImage(&gt_img);
        iftDestroyImage(&cand_img);
        iftDestroyImage(&orig_img);
        free(overlap_count);
    }
    fclose(f);
    // Write candidates dataset
    iftWriteOPFDataSet(cand_dataset, argv[4]);
    // Free
    iftDestroyDataSet(&cand_dataset);
    iftDestroyAdjRel(&A);
    iftDestroyDir(&gt_dir);
    iftDestroyDir(&orig_dir);
    iftDestroyDir(&cand_dir);

    /* ---------------------------------------------------------- */

    MemDinFinal = iftMemoryUsed();
    if (MemDinInicial!=MemDinFinal)
        printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
               MemDinInicial,MemDinFinal);

    return(0);
}




