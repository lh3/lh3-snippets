/* I copied the source code somewhere in ~2014, but I forgot the source. Will add when I find. */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define RADIX 2.0

/*************************
 * balance a real matrix *
 *************************/

static void balanc(double **a, int n)
{
	int             i, j, last = 0;
	double          s, r, g, f, c, sqrdx;

	sqrdx = RADIX * RADIX;
	while (last == 0) {
		last = 1;
		for (i = 0; i < n; i++) {
			r = c = 0.0;
			for (j = 0; j < n; j++)
				if (j != i) {
					c += fabs(a[j][i]);
					r += fabs(a[i][j]);
				}
			if (c != 0.0 && r != 0.0) {
				g = r / RADIX;
				f = 1.0;
				s = c + r;
				while (c < g) {
					f *= RADIX;
					c *= sqrdx;
				}
				g = r * RADIX;
				while (c > g) {
					f /= RADIX;
					c /= sqrdx;
				}
				if ((c + r) / f < 0.95 * s) {
					last = 0;
					g = 1.0 / f;
					for (j = 0; j < n; j++)
						a[i][j] *= g;
					for (j = 0; j < n; j++)
						a[j][i] *= f;
				}
			}
		}
	}
}

#define SWAP(a,b) do { double t = (a); (a) = (b); (b) = t; } while (0)

/*****************************************************
 * convert a non-symmetric matrix to Hessenberg form *
 *****************************************************/

static void elmhes(double **a, int n)
{
	int             i, j, m;
	double          y, x;

	for (m = 1; m < n - 1; m++) {
		x = 0.0;
		i = m;
		for (j = m; j < n; j++) {
			if (fabs(a[j][m - 1]) > fabs(x)) {
				x = a[j][m - 1];
				i = j;
			}
		}
		if (i != m) {
			for (j = m - 1; j < n; j++)
				SWAP(a[i][j], a[m][j]);
			for (j = 0; j < n; j++)
				SWAP(a[j][i], a[j][m]);
		}
		if (x != 0.0) {
			for (i = m + 1; i < n; i++) {
				if ((y = a[i][m - 1]) != 0.0) {
					y /= x;
					a[i][m - 1] = y;
					for (j = m; j < n; j++)
						a[i][j] -= y * a[m][j];
					for (j = 0; j < n; j++)
						a[j][m] += y * a[j][i];
				}
			}
		}
	}
}

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

/**************************************
 * QR algorithm for Hessenberg matrix *
 **************************************/

static void hqr(double **a, int n, double *wr, double *wi)
{
	int             nn, m, l, k, j, its, i, mmin;
	double          z, y, x, w, v, u, t, s, r, q, p, anorm;

	p = q = r = 0.0;
	anorm = 0.0;
	for (i = 0; i < n; i++)
		for (j = i - 1 > 0 ? i - 1 : 0; j < n; j++)
			anorm += fabs(a[i][j]);
	nn = n - 1;
	t = 0.0;
	while (nn >= 0) {
		its = 0;
		do {
			for (l = nn; l > 0; l--) {
				s = fabs(a[l - 1][l - 1]) + fabs(a[l][l]);
				if (s == 0.0)
					s = anorm;
				if (fabs(a[l][l - 1]) + s == s) {
					a[l][l - 1] = 0.0;
					break;
				}
			}
			x = a[nn][nn];
			if (l == nn) {
				wr[nn] = x + t;
				wi[nn--] = 0.0;
			} else {
				y = a[nn - 1][nn - 1];
				w = a[nn][nn - 1] * a[nn - 1][nn];
				if (l == nn - 1) {
					p = 0.5 * (y - x);
					q = p * p + w;
					z = sqrt(fabs(q));
					x += t;
					if (q >= 0.0) {
						z = p + SIGN(z, p);
						wr[nn - 1] = wr[nn] = x + z;
						if (z != 0.0)
							wr[nn] = x - w / z;
						wi[nn - 1] = wi[nn] = 0.0;
					} else {
						wr[nn - 1] = wr[nn] = x + p;
						wi[nn - 1] = -(wi[nn] = z);
					}
					nn -= 2;
				} else {
					if (its == 30) {
						fprintf(stderr, "[hqr] too many iterations.\n");
						break;
					}
					if (its == 10 || its == 20) {
						t += x;
						for (i = 0; i < nn + 1; i++)
							a[i][i] -= x;
						s = fabs(a[nn][nn - 1]) + fabs(a[nn - 1][nn - 2]);
						y = x = 0.75 * s;
						w = -0.4375 * s * s;
					}
					++its;
					for (m = nn - 2; m >= l; m--) {
						z = a[m][m];
						r = x - z;
						s = y - z;
						p = (r * s - w) / a[m + 1][m] + a[m][m + 1];
						q = a[m + 1][m + 1] - z - r - s;
						r = a[m + 2][m + 1];
						s = fabs(p) + fabs(q) + fabs(r);
						p /= s;
						q /= s;
						r /= s;
						if (m == l)
							break;
						u = fabs(a[m][m - 1]) * (fabs(q) + fabs(r));
						v = fabs(p) * (fabs(a[m - 1][m - 1]) + fabs(z) + fabs(a[m + 1][m + 1]));
						if (u + v == v)
							break;
					}
					for (i = m; i < nn - 1; i++) {
						a[i + 2][i] = 0.0;
						if (i != m)
							a[i + 2][i - 1] = 0.0;
					}
					for (k = m; k < nn; k++) {
						if (k != m) {
							p = a[k][k - 1];
							q = a[k + 1][k - 1];
							r = 0.0;
							if (k + 1 != nn)
								r = a[k + 2][k - 1];
							if ((x = fabs(p) + fabs(q) + fabs(r)) != 0.0) {
								p /= x;
								q /= x;
								r /= x;
							}
						}
						if ((s = SIGN(sqrt(p * p + q * q + r * r), p)) != 0.0) {
							if (k == m) {
								if (l != m)
									a[k][k - 1] = -a[k][k - 1];
							} else
								a[k][k - 1] = -s * x;
							p += s;
							x = p / s;
							y = q / s;
							z = r / s;
							q /= p;
							r /= p;
							for (j = k; j < nn + 1; j++) {
								p = a[k][j] + q * a[k + 1][j];
								if (k + 1 != nn) {
									p += r * a[k + 2][j];
									a[k + 2][j] -= p * z;
								}
								a[k + 1][j] -= p * y;
								a[k][j] -= p * x;
							}
							mmin = nn < k + 3 ? nn : k + 3;
							for (i = l; i < mmin + 1; i++) {
								p = x * a[i][k] + y * a[i][k + 1];
								if (k != (nn)) {
									p += z * a[i][k + 2];
									a[i][k + 2] -= p * r;
								}
								a[i][k + 1] -= p * q;
								a[i][k] -= p;
							}
						}
					}
				}
			}
		} while (l + 1 < nn);
	}
}

/*********************************************************
 * calculate eigenvalues for a non-symmetric real matrix *
 *********************************************************/

void n_eigen(double *_a, int n, double *wr, double *wi)
{
	int             i;
	double        **a = (double **) calloc(n, sizeof(void *));
	for (i = 0; i < n; ++i)
		a[i] = _a + i * n;
	balanc(a, n);
	elmhes(a, n);
	hqr(a, n, wr, wi);
	free(a);
}

/* convert a symmetric matrix to tridiagonal form */

#define SQR(a) ((a)*(a))

static double pythag(double a, double b)
{
	double absa, absb;
	absa = fabs(a);
	absb = fabs(b);
	if (absa > absb) return absa * sqrt(1.0 + SQR(absb / absa));
	else return (absb == 0.0 ? 0.0 : absb * sqrt(1.0 + SQR(absa / absb)));
}

static void tred2(double **a, int n, double *d, double *e)
{
	int             l, k, j, i;
	double          scale, hh, h, g, f;

	for (i = n - 1; i > 0; i--) {
		l = i - 1;
		h = scale = 0.0;
		if (l > 0) {
			for (k = 0; k < l + 1; k++)
				scale += fabs(a[i][k]);
			if (scale == 0.0)
				e[i] = a[i][l];
			else {
				for (k = 0; k < l + 1; k++) {
					a[i][k] /= scale;
					h += a[i][k] * a[i][k];
				}
				f = a[i][l];
				g = (f >= 0.0 ? -sqrt(h) : sqrt(h));
				e[i] = scale * g;
				h -= f * g;
				a[i][l] = f - g;
				f = 0.0;
				for (j = 0; j < l + 1; j++) {
					/* Next statement can be omitted if eigenvectors not wanted */
					a[j][i] = a[i][j] / h;
					g = 0.0;
					for (k = 0; k < j + 1; k++)
						g += a[j][k] * a[i][k];
					for (k = j + 1; k < l + 1; k++)
						g += a[k][j] * a[i][k];
					e[j] = g / h;
					f += e[j] * a[i][j];
				}
				hh = f / (h + h);
				for (j = 0; j < l + 1; j++) {
					f = a[i][j];
					e[j] = g = e[j] - hh * f;
					for (k = 0; k < j + 1; k++)
						a[j][k] -= (f * e[k] + g * a[i][k]);
				}
			}
		} else
			e[i] = a[i][l];
		d[i] = h;
	}
	/* Next statement can be omitted if eigenvectors not wanted */
	d[0] = 0.0;
	e[0] = 0.0;
	/* Contents of this loop can be omitted if eigenvectors not wanted except for statement d[i]=a[i][i]; */
	for (i = 0; i < n; i++) {
		l = i;
		if (d[i] != 0.0) {
			for (j = 0; j < l; j++) {
				g = 0.0;
				for (k = 0; k < l; k++)
					g += a[i][k] * a[k][j];
				for (k = 0; k < l; k++)
					a[k][j] -= g * a[k][i];
			}
		}
		d[i] = a[i][i];
		a[i][i] = 1.0;
		for (j = 0; j < l; j++)
			a[j][i] = a[i][j] = 0.0;
	}
}

/* calculate the eigenvalues and eigenvectors of a symmetric tridiagonal matrix */
static void tqli(double *d, double *e, int n, double **z)
{
	int             m, l, iter, i, k;
	double          s, r, p, g, f, dd, c, b;

	for (i = 1; i < n; i++)
		e[i - 1] = e[i];
	e[n - 1] = 0.0;
	for (l = 0; l < n; l++) {
		iter = 0;
		do {
			for (m = l; m < n - 1; m++) {
				dd = fabs(d[m]) + fabs(d[m + 1]);
				if (fabs(e[m]) + dd == dd)
					break;
			}
			if (m != l) {
				if (iter++ == 30) {
					fprintf(stderr, "[tqli] Too many iterations in tqli.\n");
					break;
				}
				g = (d[l + 1] - d[l]) / (2.0 * e[l]);
				r = pythag(g, 1.0);
				g = d[m] - d[l] + e[l] / (g + SIGN(r, g));
				s = c = 1.0;
				p = 0.0;
				for (i = m - 1; i >= l; i--) {
					f = s * e[i];
					b = c * e[i];
					e[i + 1] = (r = pythag(f, g));
					if (r == 0.0) {
						d[i + 1] -= p;
						e[m] = 0.0;
						break;
					}
					s = f / r;
					c = g / r;
					g = d[i + 1] - p;
					r = (d[i] - g) * s + 2.0 * c * b;
					d[i + 1] = g + (p = s * r);
					g = c * r - b;
					/* Next loop can be omitted if eigenvectors not wanted */
					for (k = 0; k < n; k++) {
						f = z[k][i + 1];
						z[k][i + 1] = s * z[k][i] + c * f;
						z[k][i] = c * z[k][i] - s * f;
					}
				}
				if (r == 0.0 && i >= l)
					continue;
				d[l] -= p;
				e[l] = g;
				e[m] = 0.0;
			}
		} while (m != l);
	}
}

int n_eigen_symm(double *_a, int n, double *eval)
{
	double **a, *e;
	int i;
	a = (double**)calloc(n, sizeof(void*));
	e = (double*)calloc(n, sizeof(double));
	for (i = 0; i < n; ++i) a[i] = _a + i * n;
	tred2(a, n, eval, e);
	tqli(eval, e, n, a);
	free(a); free(e);
	return 0;
}

#ifdef LH3_EIGEN_MAIN

int main(void)
{
	int             i, j;
	static double   u[5], v[5];
	static double   a[5][5] = {{1.0, 6.0, -3.0, -1.0, 7.0},
	{8.0, -15.0, 18.0, 5.0, 4.0}, {-2.0, 11.0, 9.0, 15.0, 20.0},
	{-13.0, 2.0, 21.0, 30.0, -6.0}, {17.0, 22.0, -5.0, 3.0, 6.0}};

	static double b[5][5]={ {10.0,1.0,2.0,3.0,4.0},
							{1.0,9.0,-1.0,2.0,-3.0},
							{2.0,-1.0,7.0,3.0,-5.0},
							{3.0,2.0,3.0,12.0,-1.0},
							{4.0,-3.0,-5.0,-1.0,15.0}};


	printf("MAT H IS:\n");
	for (i = 0; i <= 4; i++) {
		for (j = 0; j <= 4; j++)
			printf("%13.7e ", a[i][j]);
		printf("\n");
	}
	printf("\n");
	n_eigen(a[0], 5, u, v);
	for (i = 0; i <= 4; i++)
		printf("%13.7e +J %13.7e\n", u[i], v[i]);
	printf("\n");

	printf("\n\n");
	n_eigen_symm(b[0], 5, u);
	for (i = 0; i <= 4; i++)
		printf("%13.7e\n", u[i]);
	printf("\n");
	for (i = 0; i <= 4; i++) {
		for (j = 0; j <= 4; j++)
			printf("%12.6e ", b[i][j]);
		printf("\n");
	}
	printf("\n");
	
	return 0;
}

/* correct output:

4.2961008e+01 +J 0.0000000e+00
-6.6171383e-01 +J 0.0000000e+00
-1.5339638e+01 +J -6.7556929e+00
-1.5339638e+01 +J 6.7556929e+00
1.9379983e+01 +J 0.0000000e+00

*/

#endif
