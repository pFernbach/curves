

#include "load_problem.h"

#include <string>
#include <iostream>
#include <cmath>

using namespace std;

namespace spline
{

bool QuasiEqual(const double a, const double b, const float margin)
{
	if ((a <= 0 && b <= 0) || (a >= 0 && b>= 0))
	{
        return (abs(a-b)) <= margin;
	}
	else
	{
		return abs(a) + abs(b) <= margin;
	}
}

const double margin = 0.001;

} // namespace spline

using namespace spline;

ostream& operator<<(ostream& os, const point_t& pt)
{
    os << "(" << pt.x() << ", " << pt.y() << ", " << pt.z() << ")";
    return os;
}

void ComparePoints(const Eigen::VectorXd& pt1, const Eigen::VectorXd& pt2, const std::string& errmsg, bool& error, bool notequal = false)
{
    if((pt1-pt2).norm() > margin && !notequal)
	{
		error = true;
        std::cout << errmsg << pt1.transpose() << " ; " << pt2.transpose() << std::endl;
	}
}

/*Cubic Function tests*/

void CubicFunctionTest(bool& error)
{
    std::string errMsg("In test CubicFunctionTest ; unexpected result for x ");
	point_t a(1,2,3);
	point_t b(2,3,4);
	point_t c(3,4,5);
    point_t d(3,6,7);
    t_point_t vec;
    vec.push_back(a);
    vec.push_back(b);
    vec.push_back(c);
    vec.push_back(d);
    polynom_t cf(vec.begin(), vec.end(), 0, 1);
    point_t res1;
    res1 =cf(0);
    point_t x0(1,2,3);
    ComparePoints(x0, res1, errMsg + "(0) ", error);

    point_t x1(9,15,19);
    res1 =cf(1);
    ComparePoints(x1, res1, errMsg + "(1) ", error);

    point_t x2(3.125,5.25,7.125);
    res1 =cf(0.5);
    ComparePoints(x2, res1, errMsg + "(0.5) ", error);

    vec.clear();
    vec.push_back(a);
    vec.push_back(b);
    vec.push_back(c);
    vec.push_back(d);
    polynom_t cf2(vec, 0.5, 1);
    res1 = cf2(0.5);
    ComparePoints(x0, res1, errMsg + "x3 ", error);
    error = true;
    try
    {
        cf2(0.4);
    }
    catch(...)
    {
        error = false;
    }
    if(error)
    {
        std::cout << "Evaluation of cubic cf2 error, 0.4 should be an out of range value\n";
    }
    error = true;
    try
    {
        cf2(1.1);
    }
    catch(...)
    {
        error = false;
    }
    if(error)
    {
        std::cout << "Evaluation of cubic cf2 error, 1.1 should be an out of range value\n";
    }
    if(cf.max() != 1)
    {
        error = true;
        std::cout << "Evaluation of exactCubic error, MaxBound should be equal to 1\n";
    }
    if(cf.min() != 0)
    {
        error = true;
        std::cout << "Evaluation of exactCubic error, MinBound should be equal to 1\n";
    }
}

/*bezier_curve Function tests*/

void BezierCurveTest(bool& error)
{
	std::string errMsg("In test BezierCurveTest ; unexpected result for x ");
	point_t a(1,2,3);
	point_t b(2,3,4);
	point_t c(3,4,5);
	point_t d(3,6,7);

	std::vector<point_t> params;
	params.push_back(a);
	params.push_back(b);

	// 2d curve
	bezier_curve_t cf(params.begin(), params.end());
	point_t res1;
	res1 = cf(0);
	point_t x20 = a ;
    ComparePoints(x20, res1, errMsg + "2(0) ", error);

	point_t x21 = b;
	res1 = cf(1);
    ComparePoints(x21, res1, errMsg + "2(1) ", error);

	//3d curve
	params.push_back(c);
	bezier_curve_t cf3(params.begin(), params.end());
	res1 = cf3(0);
    ComparePoints(a, res1, errMsg + "3(0) ", error);

	res1 = cf3(1);
    ComparePoints(c, res1, errMsg + "3(1) ", error);

	//4d curve
	params.push_back(d);
    bezier_curve_t cf4(params.begin(), params.end(), 2);

	res1 = cf4(2);
    ComparePoints(d, res1, errMsg + "3(1) ", error);

    //testing bernstein polynomes
    bezier_curve_t cf5(params.begin(), params.end(),2.);
    std::string errMsg2("In test BezierCurveTest ; Bernstein polynoms do not evaluate as analytical evaluation");
    for(double d = 0.; d <2.; d+=0.1)
    {
        ComparePoints( cf5.evalBernstein(d) , cf5 (d), errMsg2, error);
        ComparePoints( cf5.evalHorner(d) , cf5 (d), errMsg2, error);
        ComparePoints( cf5.compute_derivate(1).evalBernstein(d) , cf5.compute_derivate(1) (d), errMsg2, error);
        ComparePoints( cf5.compute_derivate(1).evalHorner(d) , cf5.compute_derivate(1) (d), errMsg2, error);
    }

    bool error_in(true);
	try
	{
		cf(-0.4);
	}
	catch(...)
	{
        error_in = false;
	}
    if(error_in)
	{
		std::cout << "Evaluation of bezier cf error, -0.4 should be an out of range value\n";
        error = true;
	}
    error_in = true;
	try
	{
		cf(1.1);
	}
	catch(...)
	{
        error_in = false;
	}
    if(error_in)
	{
		std::cout << "Evaluation of bezier cf error, 1.1 should be an out of range value\n";
        error = true;
	}
	if(cf.max() != 1)
	{
		error = true;
		std::cout << "Evaluation of exactCubic error, MaxBound should be equal to 1\n";
	}
	if(cf.min() != 0)
	{
		error = true;
		std::cout << "Evaluation of exactCubic error, MinBound should be equal to 1\n";
	}
}

#include <ctime>
void BezierCurveTestCompareHornerAndBernstein(bool& /*error*/)
{
    using namespace std;
    std::vector<double> values;
    for (int i =0; i < 100000; ++i)
        values.push_back(rand()/RAND_MAX);

    //first compare regular evaluation (low dim pol)
    point_t a(1,2,3);
    point_t b(2,3,4);
    point_t c(3,4,5);
    point_t d(3,6,7);
    point_t e(3,61,7);
    point_t f(3,56,7);
    point_t g(3,36,7);
    point_t h(43,6,7);
    point_t i(3,6,77);

    std::vector<point_t> params;
    params.push_back(a);
    params.push_back(b);
    params.push_back(c);

    // 3d curve
    bezier_curve_t cf(params.begin(), params.end());

    clock_t s0,e0,s1,e1,s2,e2,s3,e3;
    s0 = clock();
    for(std::vector<double>::const_iterator cit = values.begin(); cit != values.end(); ++cit)
    {
        cf(*cit);
    }
    e0 = clock();

    s1 = clock();
    for(std::vector<double>::const_iterator cit = values.begin(); cit != values.end(); ++cit)
    {
        cf.evalBernstein(*cit);
    }
    e1 = clock();

    s2 = clock();
    for(std::vector<double>::const_iterator cit = values.begin(); cit != values.end(); ++cit)
    {
        cf.evalHorner(*cit);
    }
    e2 = clock();

    s3 = clock();
    for(std::vector<double>::const_iterator cit = values.begin(); cit != values.end(); ++cit)
    {
        cf.evalDeCasteljau(*cit);
    }
    e3 = clock();


    std::cout << "time for analytical eval  " <<   double(e0 - s0) / CLOCKS_PER_SEC << std::endl;
    std::cout << "time for bernstein eval   "  <<   double(e1 - s1) / CLOCKS_PER_SEC << std::endl;
    std::cout << "time for horner eval      "     <<   double(e2 - s2) / CLOCKS_PER_SEC << std::endl;
    std::cout << "time for deCasteljau eval "     <<   double(e3 - s3) / CLOCKS_PER_SEC << std::endl;

    std::cout << "now with high order polynom "    << std::endl;


    params.push_back(d);
    params.push_back(e);
    params.push_back(f);
    params.push_back(g);
    params.push_back(h);
    params.push_back(i);

    bezier_curve_t cf2(params.begin(), params.end());

    s1 = clock();
    for(std::vector<double>::const_iterator cit = values.begin(); cit != values.end(); ++cit)
    {
        cf2.evalBernstein(*cit);
    }
    e1 = clock();

    s2 = clock();
    for(std::vector<double>::const_iterator cit = values.begin(); cit != values.end(); ++cit)
    {
        cf2.evalHorner(*cit);
    }
    e2 = clock();

    s0 = clock();
    for(std::vector<double>::const_iterator cit = values.begin(); cit != values.end(); ++cit)
    {
        cf2(*cit);
    }
    e0 = clock();

    s3 = clock();
    for(std::vector<double>::const_iterator cit = values.begin(); cit != values.end(); ++cit)
    {
        cf2.evalDeCasteljau(*cit);
    }
    e3 = clock();



    std::cout << "time for analytical eval  " <<   double(e0 - s0) / CLOCKS_PER_SEC << std::endl;
    std::cout << "time for bernstein eval   "  <<   double(e1 - s1) / CLOCKS_PER_SEC << std::endl;
    std::cout << "time for horner eval      "     <<   double(e2 - s2) / CLOCKS_PER_SEC << std::endl;
    std::cout << "time for deCasteljau eval "     <<   double(e3 - s3) / CLOCKS_PER_SEC << std::endl;

}

void BezierDerivativeCurveTest(bool& error)
{
    std::string errMsg("In test BezierDerivativeCurveTest ; unexpected result for x ");
    point_t a(1,2,3);
    point_t b(2,3,4);
    point_t c(3,4,5);

    std::vector<point_t> params;
    params.push_back(a);
    params.push_back(b);

    params.push_back(c);
    bezier_curve_t cf3(params.begin(), params.end());

    ComparePoints(cf3(0), cf3.derivate(0.,0), errMsg, error);
    ComparePoints(cf3(0), cf3.derivate(0.,1), errMsg, error, true);
    ComparePoints(point_t::Zero(), cf3.derivate(0.,100), errMsg, error);
}

void BezierDerivativeCurveTimeReparametrizationTest(bool& error)
{
    std::string errMsg("In test BezierDerivativeCurveTimeReparametrizationTest ; unexpected result for x ");
    point_t a(1,2,3);
    point_t b(2,3,4);
    point_t c(3,4,5);
    point_t d(3,4,5);
    point_t e(3,4,5);
    point_t f(3,4,5);

    std::vector<point_t> params;
    params.push_back(a);
    params.push_back(b);
    params.push_back(c);
    params.push_back(d);
    params.push_back(e);
    params.push_back(f);
    double T = 2.;
    bezier_curve_t cf(params.begin(), params.end());
    bezier_curve_t cfT(params.begin(), params.end(),T);

    ComparePoints(cf(0.5), cfT(1), errMsg, error);
    ComparePoints(cf.derivate(0.5,1), cfT.derivate(1,1) * T, errMsg, error);
    ComparePoints(cf.derivate(0.5,2), cfT.derivate(1,2) * T*T, errMsg, error);
}

void BezierDerivativeCurveConstraintTest(bool& error)
{
    std::string errMsg("In test BezierDerivativeCurveConstraintTest ; unexpected result for x ");
    point_t a(1,2,3);
    point_t b(2,3,4);
    point_t c(3,4,5);

    bezier_curve_t::curve_constraints_t constraints;
    constraints.init_vel = point_t(-1,-1,-1);
    constraints.init_acc = point_t(-2,-2,-2);
    constraints.end_vel = point_t(-10,-10,-10);
    constraints.end_acc = point_t(-20,-20,-20);

    std::vector<point_t> params;
    params.push_back(a);
    params.push_back(b);

    params.push_back(c);
    bezier_curve_t cf3(params.begin(), params.end(), constraints);

    assert(cf3.degree_ == params.size() + 3);
    assert(cf3.size_   == params.size() + 4);

    ComparePoints(a, cf3(0), errMsg, error);
    ComparePoints(c, cf3(1), errMsg, error);
    ComparePoints(constraints.init_vel, cf3.derivate(0.,1), errMsg, error);
    ComparePoints(constraints.end_vel , cf3.derivate(1.,1), errMsg, error);
    ComparePoints(constraints.init_acc, cf3.derivate(0.,2), errMsg, error);
    ComparePoints(constraints.end_vel, cf3.derivate(1.,1), errMsg, error);
    ComparePoints(constraints.end_acc, cf3.derivate(1.,2), errMsg, error);
}


void BezierToPolynomConversionTest(bool& error)
{
    std::string errMsg("In test BezierToPolynomConversionTest ; unexpected result for x ");
    point_t a(1,2,3);
    point_t b(2,3,4);
    point_t c(3,4,5);
    point_t d(3,6,7);
    point_t e(3,61,7);
    point_t f(3,56,7);
    point_t g(3,36,7);
    point_t h(43,6,7);
    point_t i(3,6,77);

    std::vector<point_t> params;
    params.push_back(a);
    params.push_back(b);
    params.push_back(c);
    params.push_back(d);
    params.push_back(e);
    params.push_back(f);
    params.push_back(g);
    params.push_back(h);
    params.push_back(i);

    bezier_curve_t cf(params.begin(), params.end());
    polynom_t pol =from_bezier<bezier_curve_t, polynom_t>(cf);
    for(double i =0.; i<1.; i+=0.01)
    {
        ComparePoints(cf(i),pol(i),errMsg, error, true);
        ComparePoints(cf(i),pol(i),errMsg, error, false);
    }
}

/*Exact Cubic Function tests*/
void ExactCubicNoErrorTest(bool& error)
{
	spline::T_Waypoint waypoints;
	for(double i = 0; i <= 1; i = i + 0.2)
	{
		waypoints.push_back(std::make_pair(i,point_t(i,i,i)));
	}
	exact_cubic_t exactCubic(waypoints.begin(), waypoints.end());
	point_t res1;
	try
	{
		exactCubic(0);
		exactCubic(1);
	}
	catch(...)
	{
		error = true;
		std::cout << "Evaluation of ExactCubicNoErrorTest error\n";
	}
	error = true;
	try
	{
		exactCubic(1.2);
	}
	catch(...)
	{
		error = false;
	}
	if(error)
	{
		std::cout << "Evaluation of exactCubic cf error, 1.2 should be an out of range value\n";
	}
	if(exactCubic.max() != 1)
	{
		error = true;
		std::cout << "Evaluation of exactCubic error, MaxBound should be equal to 1\n";
	}
	if(exactCubic.min() != 0)
	{
		error = true;
		std::cout << "Evaluation of exactCubic error, MinBound should be equal to 1\n";
	}
}


/*Exact Cubic Function tests*/
void ExactCubicTwoPointsTest(bool& error)
{
	spline::T_Waypoint waypoints;
	for(double i = 0; i < 2; ++i)
	{
		waypoints.push_back(std::make_pair(i,point_t(i,i,i)));
	}
	exact_cubic_t exactCubic(waypoints.begin(), waypoints.end());

	point_t res1 = exactCubic(0);
	std::string errmsg("in ExactCubic 2 points Error While checking that given wayPoints  are crossed (expected / obtained)");
	ComparePoints(point_t(0,0,0), res1, errmsg, error);

	res1 = exactCubic(1);
	ComparePoints(point_t(1,1,1), res1, errmsg, error);
}

void ExactCubicOneDimTest(bool& error)
{
	spline::T_WaypointOne waypoints;
	point_one zero; zero(0,0) = 9;
	point_one one; one(0,0) = 14;
	point_one two; two(0,0) = 25;
	waypoints.push_back(std::make_pair(0., zero));
	waypoints.push_back(std::make_pair(1., one));
	waypoints.push_back(std::make_pair(2., two));
	exact_cubic_one exactCubic(waypoints.begin(), waypoints.end());

	point_one res1 = exactCubic(0);
	std::string errmsg("in ExactCubicOneDim Error While checking that given wayPoints  are crossed (expected / obtained)");
	ComparePoints(zero, res1, errmsg, error);

	res1 = exactCubic(1);
	ComparePoints(one, res1, errmsg, error);
}

void CheckWayPointConstraint(const std::string& errmsg, const double step, const spline::T_Waypoint& /*wayPoints*/, const exact_cubic_t* curve, bool& error )
{
    point_t res1;
    for(double i = 0; i <= 1; i = i + step)
    {
        res1 = (*curve)(i);
        ComparePoints(point_t(i,i,i), res1, errmsg, error);
    }
}

void CheckDerivative(const std::string& errmsg, const double eval_point, const std::size_t order, const point_t& target, const exact_cubic_t* curve, bool& error )
{
    point_t res1 = curve->derivate(eval_point,order);
    ComparePoints(target, res1, errmsg, error);
}


void ExactCubicPointsCrossedTest(bool& error)
{
	spline::T_Waypoint waypoints;
	for(double i = 0; i <= 1; i = i + 0.2)
	{
		waypoints.push_back(std::make_pair(i,point_t(i,i,i)));
	}
    exact_cubic_t exactCubic(waypoints.begin(), waypoints.end());
    std::string errmsg("Error While checking that given wayPoints are crossed (expected / obtained)");
    CheckWayPointConstraint(errmsg, 0.2, waypoints, &exactCubic, error);

}

void ExactCubicVelocityConstraintsTest(bool& error)
{
    spline::T_Waypoint waypoints;
    for(double i = 0; i <= 1; i = i + 0.2)
    {
        waypoints.push_back(std::make_pair(i,point_t(i,i,i)));
    }
    std::string errmsg("Error in ExactCubicVelocityConstraintsTest (1); while checking that given wayPoints are crossed (expected / obtained)");
    spline_constraints_t constraints;
    spline_deriv_constraint_t exactCubic(waypoints.begin(), waypoints.end());
    // now check that init and end velocity are 0
    CheckWayPointConstraint(errmsg, 0.2, waypoints, &exactCubic, error);
    std::string errmsg3("Error in ExactCubicVelocityConstraintsTest (2); while checking derivative (expected / obtained)");
    // now check derivatives
    CheckDerivative(errmsg3,0,1,constraints.init_vel,&exactCubic, error);
    CheckDerivative(errmsg3,1,1,constraints.end_vel,&exactCubic, error);
    CheckDerivative(errmsg3,0,2,constraints.init_acc,&exactCubic, error);
    CheckDerivative(errmsg3,1,2,constraints.end_acc,&exactCubic, error);

    constraints.end_vel = point_t(1,2,3);
    constraints.init_vel = point_t(-1,-2,-3);
    constraints.end_acc = point_t(4,5,6);
    constraints.init_acc = point_t(-4,-4,-6);
    std::string errmsg2("Error in ExactCubicVelocityConstraintsTest (3); while checking that given wayPoints are crossed (expected / obtained)");
    spline_deriv_constraint_t exactCubic2(waypoints.begin(), waypoints.end(),constraints);
    CheckWayPointConstraint(errmsg2, 0.2, waypoints, &exactCubic2, error);

    std::string errmsg4("Error in ExactCubicVelocityConstraintsTest (4); while checking derivative (expected / obtained)");
    // now check derivatives
    CheckDerivative(errmsg4,0,1,constraints.init_vel,&exactCubic2, error);
    CheckDerivative(errmsg4,1,1,constraints.end_vel ,&exactCubic2, error);
    CheckDerivative(errmsg4,0,2,constraints.init_acc,&exactCubic2, error);
    CheckDerivative(errmsg4,1,2,constraints.end_acc ,&exactCubic2, error);
}

void CheckPointOnline(const std::string& errmsg, const point_t& A, const point_t& B, const double target, const exact_cubic_t* curve, bool& error )
{
    point_t res1 = curve->operator ()(target);
    point_t ar =(res1-A); ar.normalize();
    point_t rb =(B-res1); rb.normalize();
    if(ar.dot(rb) < 0.99999)
    {
        error = true;
        std::cout << errmsg << " ; " << A.transpose() << "\n ; " << B.transpose() << "\n ; " <<
                     target << " ; " << res1.transpose() <<  std::endl;
    }
}

void EffectorTrajectoryTest(bool& error)
{
    // create arbitrary trajectory
    spline::T_Waypoint waypoints;
    for(double i = 0; i <= 10; i = i + 2)
    {
        waypoints.push_back(std::make_pair(i,point_t(i,i,i)));
    }
    helpers::exact_cubic_t* eff_traj = helpers::effector_spline(waypoints.begin(),waypoints.end(),
                                                               Eigen::Vector3d::UnitZ(),Eigen::Vector3d(0,0,2),
                                                               1,0.02,1,0.5);
    point_t zero(0,0,0);
    point_t off1(0,0,1);
    point_t off2(10,10,10.02);
    point_t end(10,10,10);
    std::string errmsg("Error in EffectorTrajectoryTest; while checking waypoints (expected / obtained)");
    std::string errmsg2("Error in EffectorTrajectoryTest; while checking derivative (expected / obtained)");
    //first check start / goal positions
    ComparePoints(zero, (*eff_traj)(0), errmsg, error);
    ComparePoints(off1, (*eff_traj)(1), errmsg, error);
    ComparePoints(off2, (*eff_traj)(9.5), errmsg, error);
    ComparePoints(end , (*eff_traj)(10), errmsg, error);

    //then check offset at start / goal positions
    // now check derivatives
    CheckDerivative(errmsg2,0,1,zero,eff_traj, error);
    CheckDerivative(errmsg2,10,1,zero ,eff_traj, error);
    CheckDerivative(errmsg2,0,2,zero,eff_traj, error);
    CheckDerivative(errmsg2,10,2,zero ,eff_traj, error);

    //check that end and init splines are line
    std::string errmsg3("Error in EffectorTrajectoryTest; while checking that init/end splines are line (point A/ point B, time value / point obtained) \n");
    for(double i = 0.1; i<1; i+=0.1)
    {
        CheckPointOnline(errmsg3,(*eff_traj)(0),(*eff_traj)(1),i,eff_traj,error);
    }

    for(double i = 9.981; i<10; i+=0.002)
    {
        CheckPointOnline(errmsg3,(*eff_traj)(9.5),(*eff_traj)(10),i,eff_traj,error);
    }
    delete eff_traj;
}

helpers::quat_t GetXRotQuat(const double theta)
{
    Eigen::AngleAxisd m (theta, Eigen::Vector3d::UnitX());
    return helpers::quat_t(Eigen::Quaterniond(m).coeffs().data());
}

double GetXRotFromQuat(helpers::quat_ref_const_t q)
{
    Eigen::Quaterniond quat (q.data());
    Eigen::AngleAxisd m (quat);
    return m.angle() / M_PI * 180.;
}

void EffectorSplineRotationNoRotationTest(bool& error)
{
    // create arbitrary trajectory
    spline::T_Waypoint waypoints;
    for(double i = 0; i <= 10; i = i + 2)
    {
        waypoints.push_back(std::make_pair(i,point_t(i,i,i)));
    }
    helpers::effector_spline_rotation eff_traj(waypoints.begin(),waypoints.end());
    helpers::config_t q_init; q_init    << 0.,0.,0.,0.,0.,0.,1.;
    helpers::config_t q_end; q_end      << 10.,10.,10.,0.,0.,0.,1.;
    helpers::config_t q_to; q_to        << 0.,0,0.02,0.,0.,0.,1.;
    helpers::config_t q_land; q_land    << 10,10, 10.02, 0, 0.,0.,1.;
    helpers::config_t q_mod; q_mod      << 6.,6.,6.,0.,0.,0.,1.;
    std::string errmsg("Error in EffectorSplineRotationNoRotationTest; while checking waypoints (expected / obtained)");
    ComparePoints(q_init , eff_traj(0),    errmsg,error);
    ComparePoints(q_to   , eff_traj(0.02), errmsg,error);
    ComparePoints(q_land , eff_traj(9.98), errmsg,error);
    ComparePoints(q_mod  , eff_traj(6),    errmsg,error);
    ComparePoints(q_end  , eff_traj(10),   errmsg,error);
}

void EffectorSplineRotationRotationTest(bool& error)
{
    // create arbitrary trajectory
    spline::T_Waypoint waypoints;
    for(double i = 0; i <= 10; i = i + 2)
    {
        waypoints.push_back(std::make_pair(i,point_t(i,i,i)));
    }
    helpers::quat_t init_quat = GetXRotQuat(M_PI);
    helpers::effector_spline_rotation eff_traj(waypoints.begin(),waypoints.end(), init_quat);
    helpers::config_t q_init =  helpers::config_t::Zero(); q_init.tail<4>() = init_quat;
    helpers::config_t q_end; q_end      << 10.,10.,10.,0.,0.,0.,1.;
    helpers::config_t q_to   = q_init; q_to(2)  +=0.02;
    helpers::config_t q_land = q_end ; q_land(2)+=0.02;
    helpers::quat_t q_mod = GetXRotQuat(M_PI_2);;
    std::string errmsg("Error in EffectorSplineRotationRotationTest; while checking waypoints (expected / obtained)");
    ComparePoints(q_init, eff_traj(0),           errmsg,error);
    ComparePoints(q_to  , eff_traj(0.02),        errmsg,error);
    ComparePoints(q_land, eff_traj(9.98),        errmsg,error);
    ComparePoints(q_mod , eff_traj(5).tail<4>(), errmsg,error);
    ComparePoints(q_end , eff_traj(10),          errmsg,error);
}

void EffectorSplineRotationWayPointRotationTest(bool& error)
{
    // create arbitrary trajectory
    spline::T_Waypoint waypoints;
    for(double i = 0; i <= 10; i = i + 2)
    {
        waypoints.push_back(std::make_pair(i,point_t(i,i,i)));
    }
    helpers::quat_t init_quat = GetXRotQuat(0);
    helpers::t_waypoint_quat_t quat_waypoints_;


    helpers::quat_t q_pi_0 = GetXRotQuat(0);
    helpers::quat_t q_pi_2 = GetXRotQuat(M_PI_2);
    helpers::quat_t q_pi   = GetXRotQuat(M_PI);

    quat_waypoints_.push_back(std::make_pair(0.4,q_pi_0));
    quat_waypoints_.push_back(std::make_pair(6,q_pi_2));
    quat_waypoints_.push_back(std::make_pair(8,q_pi));


    helpers::effector_spline_rotation eff_traj(waypoints.begin(),waypoints.end(),
                                               quat_waypoints_.begin(), quat_waypoints_.end());
    helpers::config_t q_init =  helpers::config_t::Zero(); q_init.tail<4>() = init_quat;
    helpers::config_t q_end; q_end      << 10.,10.,10.,0.,0.,0.,1.; q_end.tail<4>() = q_pi;
    helpers::config_t q_mod; q_mod.head<3>() = point_t(6,6,6) ; q_mod.tail<4>() = q_pi_2;
    helpers::config_t q_to   = q_init; q_to(2)  +=0.02;
    helpers::config_t q_land = q_end ; q_land(2)+=0.02;
    std::string errmsg("Error in EffectorSplineRotationWayPointRotationTest; while checking waypoints (expected / obtained)");
    ComparePoints(q_init, eff_traj(0),           errmsg,error);
    ComparePoints(q_to  , eff_traj(0.02),        errmsg,error);
    ComparePoints(q_land, eff_traj(9.98),        errmsg,error);
    ComparePoints(q_mod , eff_traj(6), errmsg,error);
    ComparePoints(q_end , eff_traj(10),          errmsg,error);
}

void TestReparametrization(bool& error)
{
    helpers::rotation_spline s;
    const helpers::spline_deriv_constraint_one_dim& sp = s.time_reparam_;
    if(sp.min() != 0)
    {
        std::cout << "in TestReparametrization; min value is not 0, got " << sp.min() << std::endl;
        error = true;
    }
    if(sp.max() != 1)
    {
        std::cout << "in TestReparametrization; max value is not 1, got " << sp.max() << std::endl;
        error = true;
    }
    if(sp(1)[0] != 1.)
    {
        std::cout << "in TestReparametrization; end value is not 1, got " << sp(1)[0] << std::endl;
        error = true;
    }
    if(sp(0)[0] != 0.)
    {
        std::cout << "in TestReparametrization; init value is not 0, got " << sp(0)[0] << std::endl;
        error = true;
    }
    for(double i =0; i<1; i+=0.002)
    {
        if(sp(i)[0]>sp(i+0.002)[0])
        {
            std::cout << "in TestReparametrization; reparametrization not monotonous " << sp.max() << std::endl;
            error = true;
        }
    }
}

point_t randomPoint(const double min, const double max ){
    point_t p;
    for(size_t i = 0 ; i < 3 ; ++i)
        p[i] =  (rand()/(double)RAND_MAX ) * (max-min) + min;
    return p;
}

void BezierEvalDeCasteljau(bool& error){
    using namespace std;
    std::vector<double> values;
    for (int i =0; i < 100000; ++i)
        values.push_back(rand()/RAND_MAX);

    //first compare regular evaluation (low dim pol)
    point_t a(1,2,3);
    point_t b(2,3,4);
    point_t c(3,4,5);
    point_t d(3,6,7);
    point_t e(3,61,7);
    point_t f(3,56,7);
    point_t g(3,36,7);
    point_t h(43,6,7);
    point_t i(3,6,77);

    std::vector<point_t> params;
    params.push_back(a);
    params.push_back(b);
    params.push_back(c);

    // 3d curve
    bezier_curve_t cf(params.begin(), params.end());

    for(std::vector<double>::const_iterator cit = values.begin(); cit != values.end(); ++cit)
    {
        if(cf.evalDeCasteljau(*cit) != cf(*cit)){
            error = true;
            std::cout<<" De Casteljau evaluation did not return the same value as analytical"<<std::endl;
        }
    }

    params.push_back(d);
    params.push_back(e);
    params.push_back(f);
    params.push_back(g);
    params.push_back(h);
    params.push_back(i);

    bezier_curve_t cf2(params.begin(), params.end());

    for(std::vector<double>::const_iterator cit = values.begin(); cit != values.end(); ++cit)
    {
        if(cf.evalDeCasteljau(*cit) != cf(*cit)){
            error = true;
            std::cout<<" De Casteljau evaluation did not return the same value as analytical"<<std::endl;
        }
    }

}


/**
 * @brief BezierSplitCurve test the 'split' method of bezier curve
 * @param error
 */
void BezierSplitCurve(bool& error){
    // test for degree 5
    int n = 5;
    double t_min = 0.2;
    double t_max = 10;
    for(size_t i = 0 ; i < 1 ; ++i){
        // build a random curve and split it at random time :
        //std::cout<<"build a random curve"<<std::endl;
        point_t a;
        std::vector<point_t> wps;
        for(size_t j = 0 ; j <= (std::size_t)(n) ; ++j){
            wps.push_back(randomPoint(-10.,10.));
        }
        double t = (rand()/(double)RAND_MAX )*(t_max-t_min) + t_min;
        double ts = (rand()/(double)RAND_MAX )*(t);

        bezier_curve_t c(wps.begin(), wps.end(),t);
        std::pair<bezier_curve_t,bezier_curve_t> cs = c.split(ts);
        //std::cout<<"split curve of duration "<<t<<" at "<<ts<<std::endl;

        // test on splitted curves :

        if(! ((c.degree_ == cs.first.degree_) && (c.degree_ == cs.second.degree_) )){
            error = true;
            std::cout<<" Degree of the splitted curve are not the same as the original curve"<<std::endl;
        }

        if(c.max() != (cs.first.max() + cs.second.max())){
            error = true;
            std::cout<<"Duration of the splitted curve doesn't correspond to the original"<<std::endl;
        }

        if(c(0) != cs.first(0)){
            error = true;
            std::cout<<"initial point of the splitted curve doesn't correspond to the original"<<std::endl;
        }

        if(c(t) != cs.second(cs.second.max())){
            error = true;
            std::cout<<"final point of the splitted curve doesn't correspond to the original"<<std::endl;
        }

        if(cs.first.max() != ts){
            error = true;
            std::cout<<"timing of the splitted curve doesn't correspond to the original"<<std::endl;
        }

        if(cs.first(ts) != cs.second(0.)){
            error = true;
            std::cout<<"splitting point of the splitted curve doesn't correspond to the original"<<std::endl;
        }

        // check along curve :
        double ti = 0.;
        while(ti <= ts){
            if((cs.first(ti) - c(ti)).norm() > 1e-14){
                error = true;
                std::cout<<"first splitted curve and original curve doesn't correspond, error = "<<cs.first(ti) - c(ti) <<std::endl;
            }
            ti += 0.01;
        }
        while(ti <= t){
            if((cs.second(ti-ts) - c(ti)).norm() > 1e-14){
                error = true;
                std::cout<<"second splitted curve and original curve doesn't correspond, error = "<<cs.second(ti-ts) - c(ti)<<std::endl;
            }
            ti += 0.01;
        }
    }
}

/**
 * @brief BezierLinearProblemTests test the generation of linear / quadratic problems with
 * variable control points bezier curves
 * @param error
 */

using namespace spline::optimization;


var_pair_t setup_control_points(const std::size_t degree,
                          const constraint_flag flag,
                          const point_t& initPos = point_t(),
                          const point_t& endPos  = point_t(),
                          const constraint_linear& constraints = constraint_linear(),
                          const double totalTime = 1.)
{
    problem_definition_t pDef;
    pDef.curveConstraints = constraints;
    pDef.start = initPos; pDef.end = endPos;
    pDef.flag = flag;
    pDef.totalTime = totalTime;
    pDef.degree = degree;
    problem_data_t pData = setup_control_points<point_t, 3, double>(pDef);
    return std::make_pair(pData.variables_,
                          std::make_pair(pData.startVariableIndex, pData.numVariables));
}

enum vartype
{
    variable,
    constant
};

bool isVar(const linear_variable_t& var)
{
    return var.A_ == linear_variable_t::matrix_t::Identity() &&
           var.b_ == linear_variable_t::point_t::Zero();
}

bool isConstant(const linear_variable_t& var)
{
    return var.A_ == linear_variable_t::matrix_t::Zero() &&
           var.b_ != linear_variable_t::point_t::Zero();
}

/*bool isMixed(const linear_variable_t& var)
{
    return var.A_ != linear_variable_t::matrix_t::Zero() &&
           var.b_ != linear_variable_t::point_t::Zero();
}*/

bool checkValue(const linear_variable_t& var, const vartype vart)
{
    if (vart == constant)
        return isConstant(var);
    else
        return isVar(var);
}

void checksequence(const T_linear_variable_t& vars, vartype* expected,
                   const std::string testname, bool& error)
{
    int i =0;
    for(variables_t::CIT_var_t cit = vars.begin();
        cit != vars.end(); ++cit, ++i)
    {
        if(! checkValue(*cit, expected[i]))
        {
            std::cout << "in test: " << testname << ": wrong type for variable at position " << i << std::endl;
            error = true;
        }
    }
}

void checkNumVar(const T_linear_variable_t& vars, const std::size_t expected,
                 const std::string testname, bool& error)
{
    if(vars.size() != expected)
    {
        error = true;
        std::cout << "incorrect number of variables in "
                  << testname << "(" << expected << "," << vars.size() << ")" << std::endl;
    }
}

void checkPair(const pair_size_t pair, const std::size_t start_index, const std::size_t num_vars,
                 const std::string testname, bool& error)
{
    if(pair.first != start_index)
    {
        error = true;
        std::cout << "incorrect starting index for variablesin "
                  << testname << "(" << start_index << "," << pair.first << ")" << std::endl;
    }
    if(pair.second != num_vars)
    {
        error = true;
        std::cout << "incorrect number of identified variablesin "
                  << testname << "(" << num_vars << "," << pair.second << ")" << std::endl;
    }
}

void BezierLinearProblemsetup_control_pointsNoConstraint(bool& error){
    constraint_flag flag = optimization::NONE;
    var_pair_t res_no_constraints = setup_control_points(5, flag);
    T_linear_variable_t& vars = res_no_constraints.first;
    vartype exptecdvars [] = {variable,variable,variable,variable,variable,variable};
    checkNumVar(vars, 6, "setup_control_pointsNoConstraint", error);
    checksequence(vars,exptecdvars,"setup_control_pointsNoConstraint", error);
}

constraint_linear makeConstraint()
{
    point_t init_pos = point_t(1.,1.,1.);
    constraint_linear cl;
    init_pos*=2;
    cl.init_vel = init_pos;
    init_pos*=2;
    cl.init_acc = init_pos;
    init_pos*=2;
    cl.end_acc = init_pos;
    init_pos*=2;
    cl.end_vel = init_pos;
    return cl;
}

void BezierLinearProblemsetup_control_pointsVarCombinatorialInit(bool& error){
    constraint_flag flag = optimization::INIT_POS;
    point_t init_pos = point_t(1.,1.,1.);
    var_pair_t res = setup_control_points(5, flag,init_pos);
    T_linear_variable_t& vars = res.first;
    vartype exptecdvars [] = {constant,variable,variable,variable,variable,variable};
    checkNumVar(vars, 6, "VarCombinatorialInit", error);
    checksequence(vars,exptecdvars,"VarCombinatorialInit", error);
    checkPair(res.second, 1, 5, "VarCombinatorialInit", error);

    constraint_linear constraints = makeConstraint();
    flag = INIT_POS | INIT_VEL;
    res = setup_control_points(5, flag,init_pos,point_t(),constraints);
    vars = res.first;
    vartype exptecdvar1 [] = {constant,constant,variable,variable,variable,variable};
    checkNumVar(vars, 6, "VarCombinatorialInit", error);
    checksequence(vars,exptecdvar1,"VarCombinatorialInit", error);
    checkPair(res.second, 2, 4, "VarCombinatorialInit", error);

    flag = INIT_POS | INIT_VEL | INIT_ACC;
    res = setup_control_points(5, flag,init_pos,point_t(),constraints);
    vars = res.first;
    vartype exptecdvar2 [] = {constant,constant,constant,variable,variable,variable};
    checkNumVar(vars, 6, "VarCombinatorialInit", error);
    checksequence(vars,exptecdvar2,"VarCombinatorialInit", error);
    checkPair(res.second, 3, 3, "VarCombinatorialInit", error);

    flag = INIT_VEL;
    res = setup_control_points(5, flag,init_pos,point_t(),constraints);
    vars = res.first;
    vartype exptecdvar3 [] = {variable,variable,variable,variable,variable,variable};
    checkNumVar(vars, 6, "VarCombinatorialInit", error);
    checksequence(vars,exptecdvar3,"VarCombinatorialInit", error);
    checkPair(res.second, 0, 6, "VarCombinatorialInit", error);

    flag = INIT_ACC;
    res = setup_control_points(5, flag,init_pos,point_t(),constraints);
    vars = res.first;
    vartype exptecdvar4 [] = {variable,variable,variable,variable,variable,variable};
    checkNumVar(vars, 6, "VarCombinatorialInit", error);
    checksequence(vars,exptecdvar4,"VarCombinatorialInit", error);
    checkPair(res.second, 0, 6, "VarCombinatorialInit", error);

    flag = INIT_ACC | INIT_VEL;
    res = setup_control_points(5, flag,init_pos,point_t(),constraints);
    vars = res.first;
    vartype exptecdvar5 [] = {variable,variable,variable,variable,variable,variable};
    checkNumVar(vars, 6, "VarCombinatorialInit", error);
    checksequence(vars,exptecdvar5,"VarCombinatorialInit", error);
    checkPair(res.second, 0, 6, "VarCombinatorialInit", error);


    bool err = true;
    try
    {
        flag = INIT_POS | INIT_VEL;
        res = setup_control_points(1, flag,init_pos,point_t(),constraints);
    }
    catch(...)
    {
        err = false;
    }
    if(err)
    {
        error = true;
        std::cout << "exception should be raised when degree of bezier curve is not high enough to handle constraints " << std::endl;
    }
}

void BezierLinearProblemsetup_control_pointsVarCombinatorialEnd(bool& error){
    constraint_flag flag = optimization::END_POS;
    point_t init_pos = point_t(1.,1.,1.);
    var_pair_t res = setup_control_points(5, flag,init_pos);
    T_linear_variable_t& vars = res.first;
    vartype exptecdvars [] = {variable,variable,variable,variable,variable,constant};
    checkNumVar(vars, 6, "VarCombinatorialEnd", error);
    checksequence(vars,exptecdvars,"VarCombinatorialEnd", error);
    checkPair(res.second, 0, 5, "VarCombinatorialEnd", error);

    constraint_linear constraints = makeConstraint();
    flag = END_POS | END_VEL;
    res = setup_control_points(5, flag,init_pos,init_pos,constraints);
    vars = res.first;
    vartype exptecdvar1 [] = {variable,variable,variable,variable,constant,constant};
    checkNumVar(vars, 6, "VarCombinatorialEnd", error);
    checksequence(vars,exptecdvar1,"VarCombinatorialEnd", error);
    checkPair(res.second, 0, 4, "VarCombinatorialEnd", error);

    flag = END_POS | END_VEL | END_ACC;
    res = setup_control_points(5, flag,init_pos,init_pos,constraints);
    vars = res.first;
    vartype exptecdvar2 [] = {variable,variable,variable,constant,constant,constant};
    checkNumVar(vars, 6, "VarCombinatorialEnd", error);
    checksequence(vars,exptecdvar2,"VarCombinatorialEnd", error);
    checkPair(res.second, 0, 3, "VarCombinatorialEnd", error);

    flag = END_VEL;
    res = setup_control_points(5, flag,init_pos,init_pos,constraints);
    vars = res.first;
    vartype exptecdvar3 [] = {variable,variable,variable,variable,variable,variable};
    checkNumVar(vars, 6, "VarCombinatorialEnd", error);
    checksequence(vars,exptecdvar3,"VarCombinatorialEnd", error);
    checkPair(res.second, 0, 6, "VarCombinatorialEnd", error);

    flag = END_ACC;
    res = setup_control_points(5, flag,init_pos,init_pos,constraints);
    vars = res.first;
    vartype exptecdvar4 [] = {variable,variable,variable,variable,variable,variable};
    checkNumVar(vars, 6, "VarCombinatorialEnd", error);
    checksequence(vars,exptecdvar4,"VarCombinatorialEnd", error);
    checkPair(res.second, 0, 6, "VarCombinatorialEnd", error);

    flag = END_ACC | END_VEL;
    res = setup_control_points(5, flag,init_pos,init_pos,constraints);
    vars = res.first;
    vartype exptecdvar5 [] = {variable,variable,variable,variable,variable,variable};
    checkNumVar(vars, 6, "VarCombinatorialEnd", error);
    checksequence(vars,exptecdvar5,"VarCombinatorialEnd", error);
    checkPair(res.second, 0, 6, "VarCombinatorialEnd", error);


    bool err = true;
    try
    {
        flag = END_ACC | END_VEL;
        res = setup_control_points(1, flag,init_pos,point_t(),constraints);
    }
    catch(...)
    {
        err = false;
    }
    if(err)
    {
        error = true;
        std::cout << "exception should be raised when degree of bezier curve is not high enough to handle constraints " << std::endl;
    }
}


void BezierLinearProblemsetup_control_pointsVarCombinatorialMix(bool& error){
    constraint_flag flag = END_POS | INIT_POS;
    point_t init_pos = point_t(1.,1.,1.);
    var_pair_t res = setup_control_points(5, flag,init_pos);
    T_linear_variable_t& vars = res.first;
    vartype exptecdvars [] = {constant,variable,variable,variable,variable,constant};
    checkNumVar(vars, 6, "VarCombinatorialMix", error);
    checksequence(vars,exptecdvars,"VarCombinatorialMix", error);
    checkPair(res.second, 1, 4, "VarCombinatorialMix", error);

    constraint_linear constraints = makeConstraint();
    flag = END_POS | END_VEL | INIT_VEL | INIT_POS;
    res = setup_control_points(5, flag,init_pos,init_pos,constraints);
    vars = res.first;
    vartype exptecdvar1 [] = {constant,constant,variable,variable,constant,constant};
    checkNumVar(vars, 6, "VarCombinatorialMix", error);
    checksequence(vars,exptecdvar1,"VarCombinatorialMix", error);
    checkPair(res.second, 2, 2, "VarCombinatorialMix", error);

    flag = END_POS | END_VEL | END_ACC | INIT_VEL | INIT_POS;
    res = setup_control_points(5, flag,init_pos,init_pos,constraints);
    vars = res.first;
    vartype exptecdvar2 [] = {constant,constant,variable,constant,constant,constant};
    checkNumVar(vars, 6, "VarCombinatorialMix", error);
    checksequence(vars,exptecdvar2,"VarCombinatorialMix", error);
    checkPair(res.second, 2, 1, "VarCombinatorialMix", error);

    flag = ALL;
    res = setup_control_points(6, flag,init_pos,init_pos,constraints);
    vars = res.first;
    vartype exptecdvar3 [] = {constant,constant,constant,variable,constant,constant,constant};
    checkNumVar(vars, 7, "VarCombinatorialMix", error);
    checksequence(vars,exptecdvar3,"VarCombinatorialMix", error);
    checkPair(res.second, 3, 1, "VarCombinatorialMix", error);

    flag = END_VEL | END_ACC | INIT_VEL;
    res = setup_control_points(5, flag,init_pos,init_pos,constraints);
    vars = res.first;
    vartype exptecdvar4 [] = {variable,variable,variable,variable,variable,variable};
    checkNumVar(vars, 6, "VarCombinatorialMix", error);
    checksequence(vars,exptecdvar4,"VarCombinatorialMix", error);
    checkPair(res.second, 0, 6, "VarCombinatorialMix", error);

    flag = END_VEL | INIT_VEL;
    res = setup_control_points(5, flag,init_pos,init_pos,constraints);
    vars = res.first;
    vartype exptecdvar5 [] = {variable,variable,variable,variable,variable,variable};
    checkNumVar(vars, 6, "VarCombinatorialMix", error);
    checksequence(vars,exptecdvar5,"VarCombinatorialMix", error);
    checkPair(res.second, 0, 6, "VarCombinatorialMix", error);


    bool err = true;
    try
    {
        flag = ALL;
        res = setup_control_points(5, flag,init_pos,init_pos,constraints);
    }
    catch(...)
    {
        err = false;
    }
    if(err)
    {
        error = true;
        std::cout << "exception should be raised when degree of bezier curve is not high enough to handle constraints " << std::endl;
    }
}

void BezierLinearProblemInitInequalities(bool& error){
    constraint_flag flag = INIT_POS | END_POS;
    point_t init_pos = point_t(1.,1.,1.);
    var_pair_t res = setup_control_points(5, flag,init_pos);
    T_linear_variable_t& vars = res.first;
    vartype exptecdvars [] = {constant,variable,variable,variable,variable,constant};
    checkNumVar(vars, 6, "VarCombinatorialMix", error);
    checksequence(vars,exptecdvars,"VarCombinatorialMix", error);
    checkPair(res.second, 1, 4, "VarCombinatorialMix", error);

    constraint_linear constraints = makeConstraint();
    flag = END_POS | END_VEL | INIT_VEL | INIT_POS;
    res = setup_control_points(5, flag,init_pos,init_pos,constraints);
    vars = res.first;
    vartype exptecdvar1 [] = {constant,constant,variable,variable,constant,constant};
    checkNumVar(vars, 6, "VarCombinatorialMix", error);
    checksequence(vars,exptecdvar1,"VarCombinatorialMix", error);
    checkPair(res.second, 2, 2, "VarCombinatorialMix", error);

    flag = END_POS | END_VEL | END_ACC | INIT_VEL | INIT_POS;
    res = setup_control_points(5, flag,init_pos,init_pos,constraints);
    vars = res.first;
    vartype exptecdvar2 [] = {constant,constant,variable,constant,constant,constant};
    checkNumVar(vars, 6, "VarCombinatorialMix", error);
    checksequence(vars,exptecdvar2,"VarCombinatorialMix", error);
    checkPair(res.second, 2, 1, "VarCombinatorialMix", error);

    flag = ALL;
    res = setup_control_points(6, flag,init_pos,init_pos,constraints);
    vars = res.first;
    vartype exptecdvar3 [] = {constant,constant,constant,variable,constant,constant,constant};
    checkNumVar(vars, 7, "VarCombinatorialMix", error);
    checksequence(vars,exptecdvar3,"VarCombinatorialMix", error);
    checkPair(res.second, 3, 1, "VarCombinatorialMix", error);

    flag = END_VEL | END_ACC | INIT_VEL;
    res = setup_control_points(5, flag,init_pos,init_pos,constraints);
    vars = res.first;
    vartype exptecdvar4 [] = {variable,variable,variable,variable,variable,variable};
    checkNumVar(vars, 6, "VarCombinatorialMix", error);
    checksequence(vars,exptecdvar4,"VarCombinatorialMix", error);
    checkPair(res.second, 0, 6, "VarCombinatorialMix", error);

    flag = END_VEL | INIT_VEL;
    res = setup_control_points(5, flag,init_pos,init_pos,constraints);
    vars = res.first;
    vartype exptecdvar5 [] = {variable,variable,variable,variable,variable,variable};
    checkNumVar(vars, 6, "VarCombinatorialMix", error);
    checksequence(vars,exptecdvar5,"VarCombinatorialMix", error);
    checkPair(res.second, 0, 6, "VarCombinatorialMix", error);


    bool err = true;
    try
    {
        flag = ALL;
        res = setup_control_points(5, flag,init_pos,init_pos,constraints);
    }
    catch(...)
    {
        err = false;
    }
    if(err)
    {
        error = true;
        std::cout << "exception should be raised when degree of bezier curve is not high enough to handle constraints " << std::endl;
    }
}

static const std::string path = "../../tests/data/";

void BezierLinearProblemsetupLoadProblem(bool& error)
{
    problem_definition_t pDef = loadproblem(path+"test.pb");
    problem_data_t pData = setup_control_points<point_t, 3, double>(pDef);
    problem_t prob;
    initInequalityMatrix<point_t,3,double>(pDef,pData,prob);
}

int main(int /*argc*/, char** /*argv[]*/)
{
    std::cout << "performing tests... \n";
    bool error = false;
    CubicFunctionTest(error);
    ExactCubicNoErrorTest(error);
    ExactCubicPointsCrossedTest(error); // checks that given wayPoints are crossed
    ExactCubicTwoPointsTest(error);
    ExactCubicOneDimTest(error);
    ExactCubicVelocityConstraintsTest(error);
    EffectorTrajectoryTest(error);
    EffectorSplineRotationNoRotationTest(error);
    EffectorSplineRotationRotationTest(error);
    TestReparametrization(error);
    EffectorSplineRotationWayPointRotationTest(error);
    BezierCurveTest(error);
    BezierDerivativeCurveTest(error);
    BezierDerivativeCurveConstraintTest(error);
    BezierCurveTestCompareHornerAndBernstein(error);
    BezierDerivativeCurveTimeReparametrizationTest(error);
    BezierToPolynomConversionTest(error);
    BezierEvalDeCasteljau(error);
    BezierSplitCurve(error);
    BezierLinearProblemsetup_control_pointsNoConstraint(error);
    BezierLinearProblemsetup_control_pointsVarCombinatorialInit(error);
    BezierLinearProblemsetup_control_pointsVarCombinatorialEnd(error);
    BezierLinearProblemsetup_control_pointsVarCombinatorialMix(error);
    BezierLinearProblemsetupLoadProblem(error);
    if(error)
	{
        std::cout << "There were some errors\n";
		return -1;
	}
	else
	{
		std::cout << "no errors found \n";
		return 0;
	}
}