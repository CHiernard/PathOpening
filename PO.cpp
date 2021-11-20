#include "PO.h"

#include <algorithm>
#include <iostream>

PO::PO(std::vector<std::vector<int>> &image)
{
    input.initImage(image);
    initOutput();
    sortImage();
}

PO::~PO()
{
}

void PO::createNeighborhood(int neighborType, 
                             std::vector<int> &np, 
                             std::vector<int> &nm)
{
    // S-N
    if(neighborType == 1)
    {
        np.push_back(-input.getWidth()-1);
        np.push_back(-input.getWidth());
        np.push_back(-input.getWidth()+1);

        nm.push_back(input.getWidth()-1);
        nm.push_back(input.getWidth());
        nm.push_back(input.getWidth()+1);
    }

    //SW-NE
    else if(neighborType == 2)
    {
        np.push_back(-input.getWidth());
        np.push_back(1);
        np.push_back(-input.getWidth()+1);

        nm.push_back(input.getWidth());
        nm.push_back(-1);
        nm.push_back(input.getWidth()-1);
    }

    // W-E
    else if(neighborType == 3)
    {
        np.push_back(-input.getWidth()+1);
        np.push_back(1);
        np.push_back(input.getWidth()+1);
        
        nm.push_back(-input.getWidth()-1);
        nm.push_back(-1);
        nm.push_back(input.getWidth()-1);
    }

    // NW-SE
    else if(neighborType == 4)
    {
        np.push_back(-1);
        np.push_back(-input.getWidth());
        np.push_back(-input.getWidth()-1);
        
        nm.push_back(1);
        nm.push_back(input.getWidth());
        nm.push_back(input.getWidth()+1);
    }
}

bool PO::initInput(int L, int neighborType, std::vector<std::vector<int>> &lp, std::vector<std::vector<int>> &lm, int K)
{
    bool pathLongEnough = false;
    lp.resize(K+1);
    lm.resize(K+1);
    for(int i=0; i<input.getHeight()*input.getWidth(); ++i)
    {
        int x = input.getPositionX(i);
        int y = input.getPositionY(i);
        for(int k=0; k<=K; ++k)
        {
            if(x == 0 || y == 0 || x == input.getWidth()-1 || y == input.getHeight()-1)
            {
                lm[k].push_back(0);
                lp[k].push_back(0);
            }
            else
            {
                switch (neighborType)
                {
                case 1:
                    if(y-1 >= L-1)
                    {
                        lm[k].push_back(L-1);
                        pathLongEnough = true;
                    }
                    else
                        lm[k].push_back(y-1);
                    
                    if(input.getHeight()-1-y-1 >= L-1)
                    {
                        lp[k].push_back(L-1);
                        pathLongEnough = true;
                    }
                    else
                        lp[k].push_back(input.getHeight()-1-y-1);
                    break;

                case 2:
                    if((input.getWidth()-2)-(x-1)+(y-1) >= L)
                    {
                        lm[k].push_back(L);
                        pathLongEnough = true;
                    }
                    else
                        lm[k].push_back((input.getWidth()-2)-(x-1)+(y-1));

                    if((input.getHeight()-2)+(x-1)-(y-1) >= L)
                    {
                        lp[k].push_back(L);
                        pathLongEnough = true;
                    }
                    else
                        lp[k].push_back((input.getHeight()-2)+(x-1)-(y-1));
                    break;
                
                case 3:
                    if(x-1 >= L-1)
                    {
                        lp[k].push_back(L-1);
                        pathLongEnough = true;
                    }
                    else
                        lp[k].push_back(x-1);
                    
                    if(input.getWidth()-1-x-1 >= L-1)
                    {
                        lm[k].push_back(L-1);
                        pathLongEnough = true;
                    }
                    else
                        lm[k].push_back(input.getWidth()-1-x-1);
                    break;

                case 4:
                    if(input.getWidth()-2+input.getHeight()-2-1-(x-1)-(y-1) >= L)
                    {
                        lp[k].push_back(L);
                        pathLongEnough = true;
                    }
                    else
                        lp[k].push_back((input.getWidth()-2)+(input.getHeight()-2)-1-(x-1)-(y-1));   

                    if((x-1)+(y-1)+1 >= L)
                    {
                        lm[k].push_back(L);
                        pathLongEnough = true;
                    }
                    else
                        lm[k].push_back((x-1)+(y-1)+1);
                    break;

                default:
                    break;
                }
            }
        }
    }

    return pathLongEnough;
}

void PO::printPath(std::vector<std::vector<int>> &lp, std::vector<std::vector<int>> &lm, int K)
{
    for(int k=0; k<=K; ++k)
    {
        std::cout << "k=" << k << "\n";
        for(int i=0; i<lp[k].size(); ++i)
        {
            if(i % input.getWidth() == 0)
                std::cout << std::endl;
            if(!input.isPixelBorder(i))
                std::cout << lp[k][i] << " ";
        }
    }

    std::cout << std::endl;

    for(int k=0; k<=K; ++k)
    {
        std::cout << "k=" << k << "\n";
        for(int i=0; i<lm[k].size(); ++i)
        {
            if(i % input.getWidth() == 0)
                std::cout << std::endl;
            if(!input.isPixelBorder(i))
                std::cout << lm[k][i] << " ";
        }
    }
}

py::array PO::computePO(int L, int K, int neighborType)
{
    std::vector<int> np, nm;
    createNeighborhood(neighborType, np, nm);

    std::vector<std::vector<int>> lp, lm;
    if(initInput(L, neighborType, lp, lm, K))
    {
        // Liste des pixel à traiter
        std::vector<std::queue<Pixel*>> listM, listP;

        if(neighborType == 1)
        {
            listM.resize(input.getHeight());
            listP.resize(input.getHeight());
        }
        else if(neighborType == 3)
        {
            listM.resize(input.getWidth());
            listP.resize(input.getWidth());
        }

        std::queue<int> active;
        std::vector<bool> change(input.getWidth()*input.getHeight(), true);

        for(int i=0; i<sortedImage.size();)
        {
            int threshold = sortedImage[i]->getValue();
            while(i<sortedImage.size() && sortedImage[i]->getValue() == threshold)
            {
                int pos = sortedImage[i]->getPosition();
                if(input.isPixelValid(pos))
                {
                    input.setPixelValid(pos, false);

                    if(change[pos])
                    {
                        change[pos] = false;
                        if(threshold > output.getPixelIntensity(pos))
                            output.setPixelIntensity(pos, threshold);
                    }

                    if(neighborType == 1)
                    {
                        int y = input.getPositionY(pos);

                        for(int j=0; j<nm.size(); ++j)
                            if(!input.isPixelBorder(pos+nm[j]))
                                listM[y+1].push(input.getPixelAdress(pos+nm[j]));

                        for(int j=0; j<np.size(); ++j)
                            if(!input.isPixelBorder(pos+np[j]))
                                listP[y-1].push(input.getPixelAdress(pos+np[j]));
                    }
                    else if(neighborType == 3)
                    {
                        int x = input.getPositionX(pos);

                        for(int j=0; j<nm.size(); ++j)
                            if(!input.isPixelBorder(pos+nm[j]))
                                listM[x-1].push(input.getPixelAdress(pos+nm[j]));

                        for(int j=0; j<np.size(); ++j)
                            if(!input.isPixelBorder(pos+np[j]))
                                listP[x+1].push(input.getPixelAdress(pos+np[j]));
                    }
                }
                ++i;
            }

            if(neighborType == 1)
            {
                for(int j=0; j<listM.size()-1; ++j)
                    propagate(listM[j], listM[j+1], lm, nm, np, lm, lp, K, active);

                for(int j=listP.size()-1; j>0; --j)
                    propagate(listP[j], listP[j-1], lp, np, nm, lm, lp, K, active);
            }

            else if(neighborType == 3)
            {
                for(int j=listM.size()-1; j>0; --j)
                    propagate(listM[j], listM[j-1], lm, nm, np, lm, lp, K, active);

                for(int j=0; j< listP.size()-1; ++j)
                    propagate(listP[j], listP[j+1], lp, np, nm, lm, lp, K, active);
            }

            while(!active.empty())
            {
                int pos = active.front();
                active.pop();

                if(change[pos])
                {
                    int length = 0;
                    for(int k=0; k<=K; ++k)
                    {  
                        if(K-k-!input.isPixelValid(pos) >= 0)
                            length = std::max(length, lm[k][pos]+lp[K-k-!input.isPixelValid(pos)][pos]+1);
                    }

                    if(length < L)
                    {
                        change[pos] = false;
                        if(threshold > output.getPixelIntensity(pos))
                            output.setPixelIntensity(pos, threshold);
                    }
                }
            }

            printPath(lp, lm, K);

            std::cout << "-----------------\n";
        }
    }

    for(int i=0; i<input.getHeight()*input.getWidth(); ++i)
    {
        if(input.isPixelBorder(i))
            input.setPixelValid(i,false);
        else
            input.setPixelValid(i,true);
    }

    return py::cast(output.returnImage());
}

void PO::propagate(  std::queue<Pixel*> &queue, 
                std::queue<Pixel*> &next, 
                std::vector<std::vector<int>> &path, 
                std::vector<int> &succ, 
                std::vector<int> &pred, 
                std::vector<std::vector<int>> &lm, 
                std::vector<std::vector<int>> &lp, 
                int K,
                std::queue<int> &active)
{
    while(!queue.empty())
    {
        int position = queue.front()->getPosition();
        queue.pop();

        for(int k=0; k<=K; ++k)
        {
            int length = -1;
            if(k>0)
            {
                for(int j=0; j<pred.size(); ++j)
                    if(!input.isPixelBorder(position+pred[j]))
                        length = std::max(path[k-1][position+pred[j]], length);
            }

            for(int j=0; j<pred.size(); ++j)
                if(!input.isPixelBorder(position+pred[j]) && input.isPixelValid(position+pred[j]))
                    length = std::max(path[k][position+pred[j]], length);

            if(length+1 < path[k][position])
            {
                path[k][position] = length+1;

                for(int i=0; i<succ.size(); ++i)
                    if(!input.isPixelBorder(position+succ[i]))
                        next.push(input.getPixelAdress(position+succ[i]));

                active.push(position);
            }
        }
    }
}

void PO::sortImage()
{
    for(int i=0; i<input.getWidth()*input.getHeight(); ++i)
    {
        if(input.isPixelValid(i))
            sortedImage.push_back(input.getPixelAdress(i));
    }

    std::stable_sort(sortedImage.begin(), sortedImage.end(), [](Pixel *p1, Pixel *p2)
    {
        return p1->getValue() < p2->getValue();
    });
}

void PO::initOutput()
{
    output = input;
    for(int i=0; i<input.getHeight()*input.getWidth(); ++i)
        output.setPixelIntensity(i, 0);
}


PYBIND11_MODULE(PathOpening, m)
{
    py::class_<PO>(m, "PO")
        .def(py::init<std::vector<std::vector<int>> &>())
        .def("computePO", &PO::computePO);
}