// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef GRID_SCENARIO_HELPER_H
#define GRID_SCENARIO_HELPER_H

#include "node-distribution-scenario-interface.h"

#include <ns3/random-variable-stream.h>
#include <ns3/vector.h>

namespace ns3
{

/**
 * @brief The GridScenarioHelper class
 *
 * TODO: Documentation, tests
 */
class GridScenarioHelper : public NodeDistributionScenarioInterface
{
  public:
    /**
     * \brief GridScenarioHelper
     */
    GridScenarioHelper();

    /**
     * \brief ~GridScenarioHelper
     */
    ~GridScenarioHelper() override;

    /**
     * @brief SetHorizontalBsDistance
     */
    void SetHorizontalBsDistance(double d);

    /**
     * @brief SetVerticalBsDistance
     */
    void SetVerticalBsDistance(double d);

    /**
     * @brief SetRows
     */
    void SetRows(uint32_t r);

    /**
     * @brief SetColumns
     */
    void SetColumns(uint32_t c);

    /**
     * \brief Set starting position of the grid
     * \param [in] initialPos The starting position vector (x, y, z), where z is ignored.
     */
    void SetStartingPosition(const Vector& initialPos);

    void SetScenarioLength(double m);

    void SetScenarioHeight(double m);

    // inherited
    void CreateScenario() override;

    /**
     * Assign a fixed random variable stream number to the random variables
     * used by this model.  Return the number of streams (possibly zero) that
     * have been assigned.
     *
     * \param stream first stream index to use
     * \return the number of stream indices assigned by this model
     */
    int64_t AssignStreams(int64_t stream);

  private:
    double m_verticalBsDistance{-1.0};   //!< Distance between gnb
    double m_horizontalBsDistance{-1.0}; //!< Distance between gnb
    uint32_t m_rows{0};                  //!< Grid rows
    uint32_t m_columns{0};               //!< Grid columns
    Vector m_initialPos;                 //!< Initial Position
    double m_length{0};                  //!< Scenario length
    double m_height{0};                  //!< Scenario height
    Ptr<UniformRandomVariable> m_x;      //!< Random variable for X coordinate
    Ptr<UniformRandomVariable> m_y;      //!< Random variable for Y coordinate
};

} // namespace ns3
#endif // GRID_SCENARIO_HELPER_H
