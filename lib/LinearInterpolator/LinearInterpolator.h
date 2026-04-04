#ifndef LINEAR_INTERPOLATOR_H
#define LINEAR_INTERPOLATOR_H

class LinearInterpolator {
    public:
        LinearInterpolator();
        int calculate_distance(int raw_value);

    private:
        static const int TOTAL_DIVISIONS = 56;
        int raw_values[TOTAL_DIVISIONS] = {150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000, 1050, 1100, 1150, 1200, 1250, 1300, 1350, 1400, 1450, 1500, 1550, 1600, 1650, 1700, 1800, 1900, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000, 3100, 3200, 3300, 3400, 3500, 3600, 3700, 3800, 3900, 4000, 4100};
        int mm_values[TOTAL_DIVISIONS] = {1100, 960, 860, 750, 700, 600, 550, 517, 475, 431, 396, 375, 354, 337, 320, 303, 288, 274, 259, 246, 238, 229, 220, 211, 203, 198, 194, 191, 187, 184, 181, 177, 170, 164, 157, 150, 145, 140, 135, 131, 126, 121, 116, 111, 106, 101, 96, 91, 86, 81, 76, 70, 65, 60, 55, 50};
        float gradients[TOTAL_DIVISIONS - 1];
};


#endif