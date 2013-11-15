/*
 * Copyright © 2013 Bart Massey
 * [This program is licensed under the "MIT License"]
 * Please see the file COPYING in the source
 * distribution of this software for license terms.
 */

/* Inverted Pendulum GA */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define __USE_BSD
#include <math.h>

extern long random(void);
extern long srandom(long);

#define POSN_LIMIT 10.0
#define MUTATION_RATE 20
#define DT 0.1
#define LENGTH 10.0

int report_trace;
int report_stats;
int npop = 1000;

struct pendulum {
    double x, dx;
    double theta, dtheta;
};

/* http://en.wikipedia.org/wiki/Inverted_pendulum */
int step(struct pendulum *p, double dt, double dx) {
    p->x += dx;
    if (p->x <= -POSN_LIMIT || p->x >= POSN_LIMIT)
        return 0;
    double ddx = (dx / dt - p->dx) / dt;
    p->dx = dx / dt;
    double ddtheta =
        (-ddx * cos(p->theta) + 10 * sin(p->theta)) / LENGTH;
    p->dtheta += ddtheta * dt;
    p->theta += p->dtheta * dt;
    if (p->theta <= -M_PI/2 || p->theta >= M_PI/2)
        return 0;
    return 1;
}

struct genome {
    int id;
    int score;
    int nsteps;
    double *steps;
};

int gid = 1;

struct genome *pop;

double rand_step(void) {
    return (random() % 201 - 100) / 100.0;
}

void make_instance(struct genome *inst) {
    inst->id = gid++;
    int n = random() % 98 + 2;
    inst->nsteps = n;
    inst->steps = malloc(n * sizeof(inst->steps[0]));
    assert(inst->steps);
    for (int i = 0; i < n; i++)
        inst->steps[i] = rand_step();
}

void init_pop(void) {
    pop = malloc(npop * sizeof(pop[0]));
    assert(pop);
    for (int i = 0; i < npop; i++)
        make_instance(&pop[i]);
}

void report(struct pendulum *p, int id, double t, double dx) {
    printf("%d %4.2f: %4.2f(%.2f) %3.1f\n",
           id, t, p->x, dx, 360 * p->theta / 2 / M_PI);
}

int gen = 0;

void evaluate() {
    gen++;
    if (report_trace)
        printf("gen %d\n", gen);
    for (int i = 0; i < npop; i++) {
        struct pendulum p;
        p.x = 0;
        p.dx = 0;
        p.theta = 0; /*(random() % 5 - 2) / M_PI / 10;*/
        p.dtheta = 0;
        int score = 0;
        double t = 0;
        for (; score < pop[i].nsteps; score++) {
            if (report_trace && i == 0)
                report(&p, pop[i].id, t, pop[i].steps[score]);
            t += DT;
            int ok = step(&p, DT, pop[i].steps[score]);
            if (!ok)
                break;
        }
        pop[i].score = score;
    }
    if (report_trace)
        printf("\n");
}

int compare_score(const void *i1, const void *i2) {
    return ((struct genome *)i2)->score -
        ((struct genome *)i1)->score;
}

void select_and_breed(void) {
    qsort(pop, npop, sizeof(pop[0]), compare_score);
    if (report_stats) {
        double avg_score = 0.0;
        for (int i = 0; i < npop; i++)
            avg_score += pop[i].score;
        printf("gen %d max %d avg %g\n",
               gen, pop[0].score, avg_score / npop);
    }
    int new_pop = npop - npop / 5;
    for (int i = new_pop; i < npop; i++) {
        /* crossover */
        int j1 = random() % new_pop;
        int j2 = random() % new_pop;
        int n1 = random() % pop[j1].nsteps;
        int n2 = random() % pop[j2].nsteps;
        struct genome inst;
        inst.id = gid++;
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
                rand_step();
        /* insert */
        free(pop[i].steps);
        pop[i] = inst;
    }
}

int main(int argc, char **argv) {
    while (argc > 1) {
        if (!strcmp(argv[1], "-t")) {
            report_trace = 1;
        } else if (!strcmp(argv[1], "-s")) {
            report_stats = 1;
        else
            assert(0);
        argv++, --argc;
    }
    srandom(getpid());
    init_pop();
    while(1) {
        evaluate();
        select_and_breed();
    }
    return 0;
}
