/* procedures for evaluating Tseries */
#ifndef lint
static const char SCCSID[]="@(#)biveval.c	4.4	93/06/12	GIE	REL";
#endif
# include "projects.h"
# define NEAR_ONE	1.00001
	static projUV
w2, w;
static double ceval(struct PW_COEF *C, int n) {
	double d=0, dd=0, vd, vdd, tmp, *c;
	int j;

	for (C += n ; n-- ; --C ) {
		j = C->m;
		if (j) {
			vd = vdd = 0.;
			for (c = C->c + --j; j ; --j ) {
				vd = w2.v * (tmp = vd) - vdd + *c--;
				vdd = tmp;
			}
			d = w2.u * (tmp = d) - dd + w.v * vd - vdd + 0.5 * *c;
		} else
			d = w2.u * (tmp = d) - dd;
		dd = tmp;
	}
	j = C->m;
	if (j) {
		vd = vdd = 0.;
		for (c = C->c + --j; j ; --j ) {
			vd = w2.v * (tmp = vd) - vdd + *c--;
			vdd = tmp;
		}
		return (w.u * d - dd + 0.5 * ( w.v * vd - vdd + 0.5 * *c ));
	} else
		return (w.u * d - dd);
}
	projUV /* bivariate Chebyshev polynomial entry point */
bcheval(projUV in, Tseries *T) {
	projUV out;
		/* scale to +-1 */
 	w.u = ( in.u + in.u - T->a.u ) * T->b.u;
 	w.v = ( in.v + in.v - T->a.v ) * T->b.v;
	if (fabs(w.u) > NEAR_ONE || fabs(w.v) > NEAR_ONE) {
		out.u = out.v = HUGE_VAL;
		pj_errno = -36;
	} else { /* double evaluation */
		w2.u = w.u + w.u;
		w2.v = w.v + w.v;
		out.u = ceval(T->cu, T->mu);
		out.v = ceval(T->cv, T->mv);
	}
	return out;
}
	projUV /* bivariate power polynomial entry point */
bpseval(projUV in, Tseries *T) {
	projUV out;
	double *c, row;
	int i, m;

	out.u = out.v = 0.;
	for (i = T->mu; i >= 0; --i) {
		row = 0.;
		m = T->cu[i].m;
		if (m) {
			c = T->cu[i].c + m;
			while (m--)
				row = *--c + in.v * row;
		}
		out.u = row + in.u * out.u;
	}
	for (i = T->mv; i >= 0; --i) {
		row = 0.;
		m = T->cv[i].m;
		if (m) {
			c = T->cv[i].c + m;
			while (m--)
				row = *--c + in.v * row;
		}
		out.v = row + in.u * out.v;
	}
	return out;
}

projUV /* general entry point selecting evaluation mode */
biveval(projUV in, Tseries *T) {

    if (T->power) {
        return bpseval(in, T);
    } else {
        return bcheval(in, T);
    }
}

