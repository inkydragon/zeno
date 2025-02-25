#pragma once

#include <vector>
#include <zeno/zeno.h>

#include <openvdb/points/PointCount.h>
#include <openvdb/tree/LeafManager.h>
#include <openvdb/points/PointAdvect.h>
#include <openvdb/tools/Morphology.h>
#include <openvdb/tools/MeshToVolume.h>
#include <openvdb/openvdb.h>
#include <openvdb/points/PointCount.h>
#include <string.h>
namespace zeno {

template <typename GridT>
typename GridT::Ptr readFloatGrid(const std::string &fn) {
  openvdb::io::File file(fn);
  file.open();
  openvdb::GridPtrVecPtr my_grids = file.getGrids();
  file.close();
  int count = 0;
  typename GridT::Ptr grid;
  for (openvdb::GridPtrVec::iterator iter = my_grids->begin();
       iter != my_grids->end(); ++iter) {
    openvdb::GridBase::Ptr it = *iter;
    if ((*iter)->isType<GridT>()) {
      grid = openvdb::gridPtrCast<GridT>(*iter);
      count++;
      /// display meta data
      for (openvdb::MetaMap::MetaIterator it = grid->beginMeta();
           it != grid->endMeta(); ++it) {
        const std::string &name = it->first;
        openvdb::Metadata::Ptr value = it->second;
        std::string valueAsString = value->str();
        std::cout << name << " = " << valueAsString << std::endl;
      }
    }
  }
  std::cout << "count = " << count << std::endl;
  return grid;
}

template <typename GridT>
void writeFloatGrid(const std::string &fn, typename GridT::Ptr grid) {
  openvdb::io::File(fn).write({grid});
}

// match([](auto &gridPtr) {...})(someGeneralVdbGrid);

struct VDBGrid : zeno::IObject {
  virtual void output(std::string path) = 0;
  virtual void input(std::string path) = 0;
  virtual void setTransform(openvdb::math::Transform::Ptr const &trans) = 0;

  // using GeneralVdbGrid = variant<typename SomeGrid::Ptr, >;
  // virtual GeneralVdbGrid getGrid() = 0;
  virtual openvdb::CoordBBox evalActiveVoxelBoundingBox() = 0;
  virtual openvdb::Vec3d indexToWorld(openvdb::Coord &c) = 0;
  virtual openvdb::Vec3d worldToIndex(openvdb::Vec3d &c) = 0;
  virtual std::string getType() { return std::string(); }
  virtual void dilateTopo(int l) =0;
};

template <typename GridT>
// struct VDBGridWrapper : zeno::IObjectClone<VDBGridWrapper<GridT>, VDBGrid> {
struct VDBGridWrapper : VDBGrid {
  typename GridT::Ptr m_grid;

  VDBGridWrapper() { m_grid = GridT::create(); }

  // using VDBGrid::GeneralVdbGrid;
  // GeneralVdbGrid getGrid() override {
  //   return m_grid;
  // }

  openvdb::CoordBBox evalActiveVoxelBoundingBox() override {
    return m_grid->evalActiveVoxelBoundingBox();
  }
  openvdb::Vec3d indexToWorld(openvdb::Coord &c) override {
    return m_grid->transform().indexToWorld(c);
  }
  openvdb::Vec3d worldToIndex(openvdb::Vec3d &c) override {
    return m_grid->transform().worldToIndex(c);
  }
  virtual void output(std::string path) override {
    writeFloatGrid<GridT>(path, m_grid);
  }

  virtual void input(std::string path) override {
    m_grid = readFloatGrid<GridT>(path);
  }

  virtual void
  setTransform(openvdb::math::Transform::Ptr const &trans) override {
    m_grid->setTransform(trans);
  }
  virtual void
  dilateTopo(int l) override {
    openvdb::tools::dilateActiveValues(
      m_grid->tree(), l,
      openvdb::tools::NearestNeighbors::NN_FACE_EDGE_VERTEX);
  }

  virtual std::string getType() {
    if (std::is_same<GridT, openvdb::FloatGrid>::value) {
      return std::string("FloatGrid");
    } else if (std::is_same<GridT, openvdb::Int32Grid>::value) {
      return std::string("Int32Grid");
    } else if (std::is_same<GridT, openvdb::Vec3fGrid>::value) {
      return std::string("Vec3fGrid");
    } else if (std::is_same<GridT, openvdb::Vec3IGrid>::value) {
      return std::string("Vec3IGrid");
    } else if (std::is_same<GridT, openvdb::points::PointDataGrid>::value) {
      return std::string("PointDataGrid");
    } else {
      return std::string("");
    }
  }
};

struct TBBConcurrentIntArray : zeno::IObject {
  tbb::concurrent_vector<openvdb::Index32> m_data;
};

using VDBFloatGrid = VDBGridWrapper<openvdb::FloatGrid>;
using VDBIntGrid = VDBGridWrapper<openvdb::Int32Grid>;
using VDBFloat3Grid = VDBGridWrapper<openvdb::Vec3fGrid>;
using VDBInt3Grid = VDBGridWrapper<openvdb::Vec3IGrid>;
using VDBPointsGrid = VDBGridWrapper<openvdb::points::PointDataGrid>;

} // namespace zeno
