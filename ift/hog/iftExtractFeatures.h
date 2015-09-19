//
// Created by Alan Peixinho on 8/31/15.
//

#ifndef IFT_IFTEXTRACTFEATURES_H
#define IFT_IFTEXTRACTFEATURES_H

#define DESCRIPTOR_SIZE 64

iftImage *iftCreateBoundingBox2D(iftImage *img, iftImage *label, int val) {
    iftImage *out;
    int minX, minY, maxX, maxY;
    iftVoxel u;
    int p, i, j, origp;
    minX = INFINITY_INT;
    minY = INFINITY_INT;
    maxX = -1;
    maxY = -1;
    // Find bounding box
    for (p=0; p < label->n; p++) {
        if (label->val[p] == val) {
            u = iftGetVoxelCoord(label,p);
            if (u.x < minX)
                minX = u.x;
            else if (u.x > maxX)
                maxX = u.x;

            if (u.y < minY)
                minY = u.y;
            else if (u.y > maxY)
                maxY = u.y;
        }
    }
    out = iftCreateImage((maxX-minX+1), (maxY-minY+1), img->zsize);

    // Alloc Cb and Cr channels for color images
    if (img->Cb != NULL) {
        out->Cb = iftAllocUShortArray(img->n);
        out->Cr = iftAllocUShortArray(img->n);
    }
    // Copy pixel values
    for (i=minY; i < (maxY+1); i++) {
        for (j=minX; j < (maxX+1); j++) {
            origp = i * img->xsize + j;
            p = (i-minY) * (maxX-minX+1) + (j-minX);
            out->val[p] = img->val[origp];
            // Copy Cb and Cr bands for color images
            if (img->Cb != NULL) {
                out->Cb[p] = img->Cb[origp];
                out->Cr[p] = img->Cr[origp];
            }
        }
    }

    return out;
}

#endif //IFT_IFTEXTRACTFEATURES_H
