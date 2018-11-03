/**
 * @file padam_update.hpp
 * @author Marcus Edel
 *
 * Implementation of Partially adaptive momentum estimation method (Padam).
 *
 * ensmallen is free software; you may redistribute it and/or modify it under
 * the terms of the 3-clause BSD license.  You should have received a copy of
 * the 3-clause BSD license along with ensmallen.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef ENSMALLEN_ADAM_PADAM_UPDATE_HPP
#define ENSMALLEN_ADAM_PADAM_UPDATE_HPP

namespace ens {

/**
 * Partially adaptive momentum estimation method (Padam),
 * adopts historical gradient information to automatically adjust the
 * learning rate.
 *
 * For more information, see the following.
 *
 * @code
 * @article{
 *   title   = {Closing the Generalization Gap of Adaptive Gradient Methods in
 *              Training Deep Neural Networks},
 *   author  = {{Chen}, J. and {Gu}, Q.},
 *   journal = {ArXiv e-prints},
 *   url     = {https://arxiv.org/abs/1806.06763}
 *   year    = {2018}
 * }
 * @endcode
 */
class PadamUpdate
{
 public:
  /**
   * Construct the Padam update policy with the given parameters.
   *
   * @param epsilon The epsilon value used to initialise the squared gradient
   *        parameter.
   * @param beta1 The smoothing parameter.
   * @param beta2 The second moment coefficient.
   * @param partial Partially adaptive parameter.
   */
  PadamUpdate(const double epsilon = 1e-8,
              const double beta1 = 0.9,
              const double beta2 = 0.999,
              const double partial = 0.25) :
      epsilon(epsilon),
      beta1(beta1),
      beta2(beta2),
      partial(partial),
      iteration(0)
  {
    // Nothing to do.
  }

  /**
   * The Initialize method is called by SGD Optimizer method before the start of
   * the iteration update process.
   *
   * @param rows Number of rows in the gradient matrix.
   * @param cols Number of columns in the gradient matrix.
   */
  void Initialize(const size_t rows, const size_t cols)
  {
    m = arma::zeros<arma::mat>(rows, cols);
    v = arma::zeros<arma::mat>(rows, cols);
    vImproved = arma::zeros<arma::mat>(rows, cols);
  }

  /**
   * Update step for Padam.
   *
   * @param iterate Parameters that minimize the function.
   * @param stepSize Step size to be used for the given iteration.
   * @param gradient The gradient matrix.
   */
  void Update(arma::mat& iterate,
              const double stepSize,
              const arma::mat& gradient)
  {
    // Increment the iteration counter variable.
    ++iteration;

    // And update the iterate.
    m *= beta1;
    m += (1 - beta1) * gradient;

    v *= beta2;
    v += (1 - beta2) * (gradient % gradient);

    const double biasCorrection1 = 1.0 - std::pow(beta1, iteration);
    const double biasCorrection2 = 1.0 - std::pow(beta2, iteration);

    // Element wise maximum of past and present squared gradients.
    vImproved = arma::max(vImproved, v);

    iterate -= (stepSize * std::sqrt(biasCorrection2) / biasCorrection1) *
        m / arma::pow(arma::sqrt(vImproved) + epsilon, partial * 2);
  }

  //! Get the value used to initialise the squared gradient parameter.
  double Epsilon() const { return epsilon; }
  //! Modify the value used to initialise the squared gradient parameter.
  double& Epsilon() { return epsilon; }

  //! Get the smoothing parameter.
  double Beta1() const { return beta1; }
  //! Modify the smoothing parameter.
  double& Beta1() { return beta1; }

  //! Get the second moment coefficient.
  double Beta2() const { return beta2; }
  //! Modify the second moment coefficient.
  double& Beta2() { return beta2; }

  //! Get the partial adaptive parameter.
  double Partial() const { return beta2; }
  //! Modify the partial adaptive parameter.
  double& Partial() { return beta2; }

 private:
  //! The epsilon value used to initialise the squared gradient parameter.
  double epsilon;

  //! The smoothing parameter.
  double beta1;

  //! The second moment coefficient.
  double beta2;

  //! Partial adaptive parameter.
  double partial;

  //! The exponential moving average of gradient values.
  arma::mat m;

  //! The exponential moving average of squared gradient values.
  arma::mat v;

  //! The optimal sqaured gradient value.
  arma::mat vImproved;

  //! The number of iterations.
  double iteration;
};

} // namespace ens

#endif