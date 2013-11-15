/* Copyright © 2013 Bart Massey */
/* Inverted Pendulum GA */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

extern long random(void);

#define PI 3.14
#define POSN_LIMIT 10.0
#define NPOP 100
#define MUTATION_RATE 20

double moi = 1.0;

struct pendulum {
    double x;
    double theta, dtheta;
};

int step(struct pendulum *p, double dt, double dx) {
    p->x += dx;
    if (p->x <= -POSN_LIMIT || p->x >= POSN_LIMIT)
        return 0;
    p->dtheta += dx * sin(p->theta) + 10.0 * (p->theta);
    p->theta += p->dtheta;
    if (p->theta <= -PI/2 || p->theta >= PI/2)
        return 0;
    return 1;
}

struct genome {
    int score;
    int nsteps;
    double *steps;
};

struct genome pop[NPOP];

void make_instance(struct genome *inst) {
    int n = random() % 98 + 2;
    inst->nsteps = n;
    inst->steps = malloc(n * sizeof(inst->steps[0]));
    assert(inst->steps);
    for (int i = 0; i < n; i++)
        inst->steps[i] = (random() % 101 - 100) / 100.0;
}

void init_pop(void) {
    for (int i = 0; i < NPOP; i++)
        make_instance(&pop[i]);
}

void evaluate() {
    for (int i = 0; i < NPOP; i++) {
        struct pendulum p;
        p.x = 0;
        p.theta = (random() % 5 - 3) / PI / 10;
        p.dtheta = 0;
        int score = 0;
        for (; score < pop[i].nsteps; score++) {
            int ok = step(&p, 1.0, pop[i].steps[score]);
            if (!ok)
                break;
        }
        pop[i].score = score;
    }
}

int compare_score(void *i1, void*i2) {
    return ((struct genome *)i2)->score -
        ((struct genome *)i1)->score;
}

void select_and_breed(void) {
    qsort(&pop, NPOP, sizeof(pop[0]), (void *)compare_score);
    int new_pop = NPOP - NPOP / 5;
    for (int i = new_pop; i < NPOP; i++) {
        /* crossover */
        int j1 = random() % new_pop;
        int j2 = random() % new_pop;
        int n1 = random() % pop[j1].nsteps;
        int n2 = random() % pop[j2].nsteps;
        struct genome inst;
        inst.nsteps = n1 + pop[j2].nsteps - n2;
        inst.steps = malloc(inst.nsteps * sizeof(inst.steps[0]));
        assert(inst.steps);
        int j = 0;
        for (; j < n1; j++)
            inst.steps[j] = pop[j1].steps[j];
        for (; j < inst.nsteps; j++)
            inst.steps[j] = pop[j2].steps[j - n1 + n2];
        /* mutation */
        if (random() % MUTATION_RATE == 0)
            inst.steps[random() % inst.nsteps] =
                (random() % 101 - 100) / 100.0;
        /* insert */
        free(pop[i].steps);
        pop[i] = inst;
    }
}

int main() {
    init_pop();
    while(1) {
        evaluate();
        select_and_breed();
    }
    return 0;
}
