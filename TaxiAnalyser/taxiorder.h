#ifndef TAXIORDER_H
#define TAXIORDER_H


class TaxiOrder
{
public:
    TaxiOrder();

    char order_id[33];
    int departure_time;
    int end_time;
    float orig_Longtitude;
    float orig_Latitude;
    float dest_Longtitude;
    float dest_Latitude;
    float fee;

    int grid_n;
};

#endif // TAXIORDER_H
