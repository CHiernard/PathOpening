#ifndef __PIXEL_H__
#define __PIXEL_H__

class Pixel
{
    public:
        Pixel();
        virtual ~Pixel();

        int getValue();
        int getPosition();
        int getPositionX();
        int getPositionY();
        bool isValid();
        bool isInsideQueue();
        bool isBorder();

        void setValue(int value);
        void setPosition(int position);
        void setPositionX(int positionX);
        void setPositionY(int positionY);
        void setValid(bool isValid);
        void setInsideQueue(bool isInside);
        void setBorder(bool isBorder);

    private:
        int value;                      // intensité du pixel
        bool valid;                     // Le pixel respecte encore le seuil ?
        bool insideQueue;               // Le pixel est présent dans la pile
        bool border;

        int position;                  // Position du pixel dans le tableau
        int positionX;
        int positionY;
};

#endif