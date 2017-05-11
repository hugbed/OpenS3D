#include "gtest/gtest.h"

#include "s3d/multiview/sampson_distance_function.h"

using s3d::StanAlignment;
using s3d::SampsonDistanceFunction;

TEST(sampson_distance_function, real_image_features_overkill_integration) {
  std::vector<Eigen::Vector3d> x;
  std::vector<Eigen::Vector3d> xp;

  x.emplace_back(196.8120, 52.7801, 1.0);
  x.emplace_back(185.9378, 97.4600, 1.0);
  x.emplace_back(227.1381, 38.4914, 1.0);
  x.emplace_back(206.1453, 103.4962, 1.0);
  x.emplace_back(204.3704, 157.9388, 1.0);
  x.emplace_back(198.8696, 79.1571, 1.0);
  x.emplace_back(250.4824, 39.9261, 1.0);
  x.emplace_back(280.1432, 139.1748, 1.0);
  x.emplace_back(241.8560, 46.6588, 1.0);
  x.emplace_back(249.2965, 25.2411, 1.0);
  x.emplace_back(257.1151, 204.9826, 1.0);
  x.emplace_back(198.1321, 111.1654, 1.0);
  x.emplace_back(226.1158, 128.1219, 1.0);
  x.emplace_back(176.8027, 79.6620, 1.0);
  x.emplace_back(199.1734, 121.4506, 1.0);
  x.emplace_back(179.7567, 91.2225, 1.0);
  x.emplace_back(220.5826, 80.2686, 1.0);
  x.emplace_back(254.7935, 211.1102, 1.0);
  x.emplace_back(257.3823, 198.6174, 1.0);
  x.emplace_back(179.8486, 117.0626, 1.0);
  x.emplace_back(188.5283, 108.9792, 1.0);
  x.emplace_back(191.3408, 138.0984, 1.0);
  x.emplace_back(221.9595, 106.0130, 1.0);
  x.emplace_back(166.0449, 109.1107, 1.0);
  x.emplace_back(164.7687, 107.3830, 1.0);
  x.emplace_back(217.5643, 134.2629, 1.0);
  x.emplace_back(184.8041, 131.4952, 1.0);
  x.emplace_back(253.5831, 188.4510, 1.0);
  x.emplace_back(181.9215, 68.3720, 1.0);
  x.emplace_back(194.3690, 127.8710, 1.0);
  x.emplace_back(129.1911, 225.0329, 1.0);
  x.emplace_back(251.3896, 188.7072, 1.0);
  x.emplace_back(195.1933, 93.3912, 1.0);
  x.emplace_back(271.5982, 138.6167, 1.0);
  x.emplace_back(245.6395, 90.0296, 1.0);
  x.emplace_back(244.7745, 200.8578, 1.0);
  x.emplace_back(204.0487, 102.8697, 1.0);
  x.emplace_back(201.8965, 137.7979, 1.0);
  x.emplace_back(267.6637, 83.0961, 1.0);
  x.emplace_back(271.0299, 221.0731, 1.0);
  x.emplace_back(253.3703, 203.3996, 1.0);
  x.emplace_back(173.0567, 133.3766, 1.0);
  x.emplace_back(266.9323, 191.8862, 1.0);
  x.emplace_back(268.9524, 206.2743, 1.0);
  x.emplace_back(255.3816, 188.8558, 1.0);
  x.emplace_back(238.7604, 225.2768, 1.0);
  x.emplace_back(266.1286, 187.0461, 1.0);
  x.emplace_back(188.0893, 113.9954, 1.0);

  xp.emplace_back(177.9248, 51.1634, 1.0);
  xp.emplace_back(165.9662, 96.3985, 1.0);
  xp.emplace_back(209.7127, 38.0998, 1.0);
  xp.emplace_back(188.5015, 103.0356, 1.0);
  xp.emplace_back(186.1846, 155.8739, 1.0);
  xp.emplace_back(180.2560, 77.7382, 1.0);
  xp.emplace_back(243.1938, 39.9053, 1.0);
  xp.emplace_back(271.0840, 138.8104, 1.0);
  xp.emplace_back(234.9161, 46.3833, 1.0);
  xp.emplace_back(243.0300, 25.0132, 1.0);
  xp.emplace_back(238.2312, 203.8984, 1.0);
  xp.emplace_back(178.1911, 110.1382, 1.0);
  xp.emplace_back(207.5074, 126.7412, 1.0);
  xp.emplace_back(156.9791, 78.7761, 1.0);
  xp.emplace_back(179.6129, 120.5365, 1.0);
  xp.emplace_back(159.8706, 90.2169, 1.0);
  xp.emplace_back(203.2863, 79.8425, 1.0);
  xp.emplace_back(235.8999, 209.7359, 1.0);
  xp.emplace_back(239.1215, 198.0190, 1.0);
  xp.emplace_back(160.0984, 115.6038, 1.0);
  xp.emplace_back(168.5789, 108.0635, 1.0);
  xp.emplace_back(171.6955, 136.9613, 1.0);
  xp.emplace_back(204.8121, 105.6817, 1.0);
  xp.emplace_back(146.4507, 108.0395, 1.0);
  xp.emplace_back(144.9408, 106.3458, 1.0);
  xp.emplace_back(199.2968, 133.5775, 1.0);
  xp.emplace_back(164.9578, 130.3698, 1.0);
  xp.emplace_back(234.8232, 187.2740, 1.0);
  xp.emplace_back(162.3514, 67.3761, 1.0);
  xp.emplace_back(174.6611, 126.8727, 1.0);
  xp.emplace_back(111.2000, 223.7443, 1.0);
  xp.emplace_back(232.5590, 187.3437, 1.0);
  xp.emplace_back(175.4510, 92.4201, 1.0);
  xp.emplace_back(262.4523, 138.1351, 1.0);
  xp.emplace_back(238.1552, 88.6110, 1.0);
  xp.emplace_back(225.8935, 199.9055, 1.0);
  xp.emplace_back(184.7442, 101.8993, 1.0);
  xp.emplace_back(182.6730, 137.2139, 1.0);
  xp.emplace_back(259.6233, 82.5002, 1.0);
  xp.emplace_back(251.9723, 218.6209, 1.0);
  xp.emplace_back(234.3885, 202.6143, 1.0);
  xp.emplace_back(154.3468, 132.9195, 1.0);
  xp.emplace_back(248.0333, 189.0962, 1.0);
  xp.emplace_back(250.0583, 204.3957, 1.0);
  xp.emplace_back(236.9342, 187.7451, 1.0);
  xp.emplace_back(219.8321, 223.8812, 1.0);
  xp.emplace_back(248.0333, 189.0962, 1.0);
  xp.emplace_back(168.1256, 112.9354, 1.0);

  StanAlignment model{-0.060726821886125215,
                      0.031560420672474818,
                      0.040187771735154088,
                      8.3586881896848784,
                      -0.00015986108011345864,
                      0,
                      0};

  std::vector<double> distances;
  distances.resize(x.size());
  SampsonDistanceFunction::ComputeDistance(x, xp, model, &distances);

  EXPECT_NEAR(distances[0], 0.17, 0.01);
  EXPECT_NEAR(distances[1], 0.10, 0.01);
  EXPECT_NEAR(distances[2], 0.00, 0.01);

  EXPECT_NEAR(distances[distances.size() - 3], 2.61, 0.01);
  EXPECT_NEAR(distances[distances.size() - 2], 0.91, 0.01);
  EXPECT_NEAR(distances[distances.size() - 1], 0.25, 0.01);
}
