#include "bsgmm.hpp"

BackgroundSubtractorGMM::BackgroundSubtractorGMM( int frameHeight, int frameWidth ) : frameHeight( frameHeight ), frameWidth( frameWidth )
{
    this->alpha = 0.003;
    this->alpha_bar = 1 - alpha;
    this->defaultVariance = 11;
    this->cT = 0.05;
    this->minVariance = 4;
    this->maxVariance = 55;
    this->cf = 0.1;
    this->cfbar = 1 - this->cf;
    this->BGSigma = 4 * 4;
    this->closeSigma = 3 * 3;
    this->prune = this->alpha * this->cT;
    this->defaultGMMCount = 4;
    this->shadowDetection = true;
    this->tau = 0.5;
    this->removeForeground = false;
    this->shadowBeBackground = false;
    pixelGMMBuffer = new PIXELGMM[frameHeight * frameWidth];
    for ( int i = 0; i < frameHeight * frameHeight; i++ )
    {
        pixelGMMBuffer[i].GMMCount = 0;
    }
    gp << "set title 'Gmm square distance map'" << std::endl;
    gp << "set dgrid3d 50,50 qnorm 2" << endl;
    gp << "set xrange [0:" + to_string( frameWidth ) + "]" << std::endl;
    gp << "set yrange [0:" + to_string( frameHeight ) + "]" << std::endl;
    gp << "set zrange [0:500]" << std::endl;
}

void BackgroundSubtractorGMM::updateFrame( uchar *inputPtr, uchar *outputPtr )
{
    PIXELGMM *curPixelGMM = this->pixelGMMBuffer;
    /* for ( int i = 0; i < this->frameWidth * this->frameHeight; i++ ) */
    gp << "splot '-' u 1:2:3 with lines" << endl;
    for ( int i = 0; i < this->frameHeight; i++ )
    {
        for ( int j = 0; j < this->frameWidth; j++ )
        {
            double red = *inputPtr++;
            double green = *inputPtr++;
            double blue = *inputPtr++;
            bool isShdw = false;
            bool isBG = isBackGround( red, green, blue, curPixelGMM, j, i );
            if ( this->shadowDetection && isBG == false )
            {
                isShdw = this->isShadow( red, green, blue, curPixelGMM );
            }
            if ( isBG )
            {
                *outputPtr = BLACK;
            }
            else
            {
                *outputPtr = isShdw ? ( this->shadowBeBackground ? BLACK : GRAY ) : WHITE;
                if ( this->removeForeground )
                {
                    *( inputPtr - 3 ) = curPixelGMM->arr[0].R;
                    *( inputPtr - 2 ) = curPixelGMM->arr[0].G;
                    *( inputPtr - 1 ) = curPixelGMM->arr[0].B;
                }
            }
            curPixelGMM++;
            outputPtr++;
        }
    }
    gp << "e" << endl;
}

bool BackgroundSubtractorGMM::isShadow( double red, double green, double blue, PIXELGMM *curPixelGMM )
{
    double totalWeight = 0;
    double numerator = 0, denominator = 0;
    for ( int GMMIndex = 0; GMMIndex < curPixelGMM->GMMCount; GMMIndex++ )
    {
        double var = curPixelGMM->arr[GMMIndex].variance;
        double R = curPixelGMM->arr[GMMIndex].R;
        double G = curPixelGMM->arr[GMMIndex].G;
        double B = curPixelGMM->arr[GMMIndex].B;
        totalWeight += curPixelGMM->arr[GMMIndex].weight;
        numerator = red * R + green * G + blue * B;
        denominator = R * R + G * G + B * B;
        if ( denominator == 0 )
        {
            break;
        }
        double a = numerator / denominator;
        if ( ( a <= 1 ) && ( a >= this->tau ) )
        {
            double dR = a * R - red;
            double dG = a * G - green;
            double dB = a * B - blue;
            double dist = ( dR * dR + dG * dG + dB * dB );
            if ( dist < this->BGSigma * var * a * a )
            {
                return true;
            }
        }
        if ( totalWeight > this->cfbar )
        {
            break;
        }
    }
    return false;
}

bool BackgroundSubtractorGMM::isBackGround( double red, double green, double blue, PIXELGMM *curPixelGMM, int x, int y )
{
    bool hitGMM = false;
    bool isBackGround = false;
    double totalWeight = 0;
    for ( int GMMIndex = 0; GMMIndex < curPixelGMM->GMMCount; GMMIndex++ )
    {
        double weight = curPixelGMM->arr[GMMIndex].weight;
        if ( hitGMM == false )
        {
            double var = curPixelGMM->arr[GMMIndex].variance;
            double R = curPixelGMM->arr[GMMIndex].R;
            double G = curPixelGMM->arr[GMMIndex].G;
            double B = curPixelGMM->arr[GMMIndex].B;
            double dR = R - red;
            double dG = G - green;
            double dB = B - blue;
            double dist = ( dR * dR + dG * dG + dB * dB );
            if ( ( totalWeight < this->cfbar ) && ( dist < this->BGSigma * var ) )
            {
                isBackGround = true;
                if ( x % 10 == 1 && y % 10 == 1 )
                {
                    gp << x << " " << y << " " << to_string( ( int )dist ) << '\n';
                }
            }
            if ( dist < this->closeSigma * var )
            {
                hitGMM = true;
                double mul = this->alpha / weight;
                weight = this->alpha_bar * weight + this->prune;
                weight += this->alpha;
                curPixelGMM->arr[GMMIndex].R -= mul * ( dR );
                curPixelGMM->arr[GMMIndex].G -= mul * ( dG );
                curPixelGMM->arr[GMMIndex].B -= mul * ( dB );
                double newVar = var + mul * ( dist - var );
                curPixelGMM->arr[GMMIndex].variance = newVar<4 ? 4 : newVar>5 * this->defaultVariance ? 5 * this->defaultVariance : newVar;
                for ( int sortIndex = GMMIndex; sortIndex > 0; sortIndex-- )
                {
                    if ( weight < ( curPixelGMM->arr[sortIndex - 1].weight ) )
                    {
                        break;
                    }
                    else
                    {
                        swap( curPixelGMM->arr[sortIndex], curPixelGMM->arr[sortIndex - 1] );
                    }
                }
            }
            else
            {
                weight = this->alpha_bar * weight + this->prune;
                if ( weight < -this->prune )
                {
                    weight = 0;
                    curPixelGMM->GMMCount--;
                }
            }
        }
        else
        {
            weight = this->alpha_bar * weight + this->prune;
            if ( weight < -this->prune )
            {
                weight = 0;
                curPixelGMM->GMMCount--;
            }
        }
        totalWeight += weight;
        curPixelGMM->arr[GMMIndex].weight = weight;
    }
    for ( int index = 0; index < curPixelGMM->GMMCount; index++ )
    {
        curPixelGMM->arr[index].weight = curPixelGMM->arr[index].weight / totalWeight;
    }
    if ( hitGMM == false )
    {
        if ( curPixelGMM->GMMCount < this->defaultGMMCount )
        {
            curPixelGMM->GMMCount++;
        }
        if ( curPixelGMM->GMMCount == 1 )
        {
            curPixelGMM->arr[curPixelGMM->GMMCount - 1].weight = 1;
        }
        else
        {
            curPixelGMM->arr[curPixelGMM->GMMCount - 1].weight = this->alpha;
        }
        double tmpTotal = 0;
        for ( int index = 0; index < curPixelGMM->GMMCount - 1; index++ )
        {
            tmpTotal += curPixelGMM->arr[index].weight;
        }
        for ( int index = 0; index < curPixelGMM->GMMCount - 1; index++ )
        {
            curPixelGMM->arr[index].weight /= tmpTotal;
        }
        curPixelGMM->arr[curPixelGMM->GMMCount - 1].R = red;
        curPixelGMM->arr[curPixelGMM->GMMCount - 1].G = green;
        curPixelGMM->arr[curPixelGMM->GMMCount - 1].B = blue;
        curPixelGMM->arr[curPixelGMM->GMMCount - 1].variance = this->defaultVariance;
        for ( int sortIndex = curPixelGMM->GMMCount - 1; sortIndex > 0; sortIndex-- )
        {
            if ( this->alpha < ( curPixelGMM->arr[sortIndex - 1].weight ) )
            {
                break;
            }
            else
            {
                swap( curPixelGMM->arr[sortIndex], curPixelGMM->arr[sortIndex - 1] );
            }
        }
    }
    return isBackGround;
}

void BackgroundSubtractorGMM::freeMem()
{
    delete[] pixelGMMBuffer;
}
