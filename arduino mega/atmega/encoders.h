
/*
 * 1) TYPES
 * 2) GLOBLAS
 * 3) INTERFACE
 * 4) INTERNALS
 */


/***** TYPES *****/

// MehanicalCombination

typedef struct {
    bool one;
    bool two;
    bool three;
} MehanicalCombination;
MehanicalCombination EMPTY_COMBINATION = {false, false, false};

bool operator==(const MehanicalCombination& meh_comb_1, const MehanicalCombination& meh_comb_2)
{
    return meh_comb_1.one == meh_comb_2.one &&
           meh_comb_1.two == meh_comb_2.two &&
           meh_comb_1.three == meh_comb_2.three;
}

bool operator!=(const MehanicalCombination& meh_comb_1, const MehanicalCombination& meh_comb_2)
{
    return !(meh_comb_1 == meh_comb_2);
}

String to_string(const MehanicalCombination& comb) {
    return (comb.one ? String("1") : String("0"))
         + (comb.two ? String("1") : String("0"))
         + (comb.three ? String("1") : String("0"));
}


// FixedPosition

typedef struct {
    MehanicalCombination combination;
    double azimuth;
    double cycles;
} FixedPosition;
FixedPosition EMPTY_POSITION = {EMPTY_COMBINATION, 0, 0};



/***** GLOBALS *****/

//TODO read screws using interrupts
//TODO put positons in an array
//TODO allow arbitrary number of fixed_positions, determined diring callibration

// volatile FixedPosition[] position = {EMPTY_POSITION, EMPTY_POSITION, EMPTY_POSITION, EMPTY_POSITION};
FixedPosition position1 = EMPTY_POSITION;
FixedPosition position2 = EMPTY_POSITION;
FixedPosition position3 = EMPTY_POSITION;
FixedPosition position4 = EMPTY_POSITION;

/*
 * Given a MehanicalCombination, point to the first FixedPosition object that matches.
 */
FixedPosition *get_position(MehanicalCombination comb)
{
    if (comb == position1.combination) {
        debugln("position1");
        return &position1;
    }
    if (comb == position2.combination) {
        debugln("position2");
        return &position2;
    }
    if (comb == position3.combination) {
        debugln("position3");
        return &position3;
    }
    if (comb == position4.combination) {
        debugln("position4");
        return &position4;
    }
    debug("PANIC1111 No FixedPosition matches given MehanicalCombination: ");
    debugln(to_string(comb));
}


/***** INTERFACE *****/

// does not read 

// writes to encoders:
typedef struct {
    double current_azimuth;
    MehanicalCombination last_combination;
} Encoders;
Encoders encoders = {NULL_AZIMUTH, EMPTY_COMBINATION};

//TODO encoders should be azimuth-agnositc (that job belongs to status.ino)
// (should store cycles instead)



/***** INTERNALS *****/

typedef struct {
    MehanicalCombination current_combination;
    long current_combination_start_cycles;
    bool read_combination;
} EncodersInternal;
EncodersInternal _encoders = {EMPTY_COMBINATION, 0, false};

typedef struct {
    long cycle;
    long last_cycle;
    int QE_old;
    int QE_oldd;
    int QE_new;
    int QE_code;
    int QE_matrix[16];
} QEncoder;
volatile QEncoder _q_encoder = {0, 0, 0, 0, 0, 0, {0, 2, 1, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 0}};


/***** PROTOTYPES *****/

void encoder_loop();

void update_QE();
int count_quadrature(int value);
int read_quadrature();
int read_quadrature_edge();

