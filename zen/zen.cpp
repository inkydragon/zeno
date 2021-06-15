#include <zen/zen.h>
#include <zen/ConditionObject.h>

namespace zen {

ZENAPI Exception::Exception(std::string const &msg) noexcept
    : msg(msg) {
}

ZENAPI Exception::~Exception() noexcept = default;

ZENAPI char const *Exception::what() const noexcept {
    return msg.c_str();
}

template <class T>
T *safe_at(std::map<std::string, std::unique_ptr<T>> const &m,
           std::string const &key, std::string const &msg) {
  auto it = m.find(key);
  if (it == m.end()) {
    throw Exception("invalid " + msg + " name: " + key);
  }
  return it->second.get();
}

template <class T>
T safe_at(std::map<std::string, T> const &m, std::string const &key,
          std::string const &msg) {
  auto it = m.find(key);
  if (it == m.end()) {
    throw Exception("invalid " + msg + " name: " + key);
  }
  return it->second;
}

template <class T, class S>
T safe_at(std::map<S, T> const &m, S const &key, std::string const &msg) {
  auto it = m.find(key);
  if (it == m.end()) {
    throw Exception("invalid " + msg + " as index");
  }
  return it->second;
}


#ifndef ZEN_FREE_IOBJECT
ZENAPI IObject::IObject() = default;
ZENAPI IObject::~IObject() = default;
#endif

ZENAPI INode::INode() = default;
ZENAPI INode::~INode() = default;

ZENAPI void INode::complete() {}

ZENAPI void INode::doApply() {
    for (auto [ds, bound]: inputBounds) {
        auto [sn, ss] = bound;
        sess->applyNode(sn);
        inputs[ds] = sess->getNodeOutput(sn, ss);
    }
    bool ok = true;
    if (has_input("COND")) {
        auto cond = get_input<zen::ConditionObject>("COND");
        if (!cond->get())
            ok = false;
    }
    if (ok)
        apply();
    set_output("DST", std::make_unique<zen::ConditionObject>());
}

ZENAPI bool INode::has_input(std::string const &id) const {
    return inputs.find(id) != inputs.end();
}

ZENAPI IObject *INode::get_input(std::string const &id) const {
    return sess->getObject(safe_at(inputs, id, "input"));
}

ZENAPI IValue INode::get_param(std::string const &id) const {
    return safe_at(params, id, "param");
}

ZENAPI void INode::set_output(std::string const &id, std::unique_ptr<IObject> &&obj) {
    auto objid = myname + "::" + id;
    sess->objects[objid] = std::move(obj);
    outputs[id] = objid;
}

ZENAPI std::string INode::set_output_ref(const std::string &id, const std::string &ref) {
    return outputs[id] = ref;
}

ZENAPI std::string INode::get_input_ref(const std::string &id) const {
    return safe_at(inputs, id, "input");
}

ZENAPI void Session::_defNodeClass(std::string const &id, std::unique_ptr<INodeClass> &&cls) {
    nodeClasses[id] = std::move(cls);
}

ZENAPI std::string Session::getNodeOutput(std::string const &sn, std::string const &ss) const {
    auto node = safe_at(nodes, sn, "node");
    return safe_at(node->outputs, ss, "node output");
}

ZENAPI IObject *Session::getObject(std::string const &id) const {
    return safe_at(objects, id, "object");
}

ZENAPI void Session::clearNodes() {
    nodes.clear();
    objects.clear();
}

ZENAPI void Session::addNode(std::string const &cls, std::string const &id) {
    if (nodes.find(id) != nodes.end())
        return;  // no add twice, to prevent output object invalid
    auto node = safe_at(nodeClasses, cls, "node class")->new_instance();
    node->sess = this;
    node->myname = id;
    nodes[id] = std::move(node);
}

ZENAPI void Session::completeNode(std::string const &id) {
    safe_at(nodes, id, "node")->complete();
}

ZENAPI void Session::applyNode(std::string const &id) {
    if (ctx->visited.find(id) != ctx->visited.end()) {
        return;
    }
    ctx->visited.insert(id);
    safe_at(nodes, id, "node")->doApply();
}

ZENAPI void Session::applyNodes(std::vector<std::string> const &ids) {
    ctx = std::make_unique<Context>();
    for (auto const &id: ids) {
        applyNode(id);
    }
    ctx = nullptr;
}

ZENAPI void Session::bindNodeInput(std::string const &dn, std::string const &ds,
        std::string const &sn, std::string const &ss) {
    safe_at(nodes, dn, "node")->inputBounds[ds] = std::pair(sn, ss);
}

ZENAPI void Session::setNodeParam(std::string const &id, std::string const &par,
        IValue const &val) {
    safe_at(nodes, id, "node")->params[par] = val;
}

ZENAPI std::string Session::dumpDescriptors() const {
  std::string res = "";
  for (auto const &[key, cls] : nodeClasses) {
    res += "DESC:" + key + ":" + cls->desc->serialize() + "\n";
  }
  return res;
}


ZENAPI Session &getSession() {
    static std::unique_ptr<Session> ptr;
    if (!ptr) {
        ptr = std::make_unique<Session>();
    }
    return *ptr;
}



ParamDescriptor::ParamDescriptor(std::string const &type,
	  std::string const &name, std::string const &defl)
      : type(type), name(name), defl(defl) {}
ParamDescriptor::~ParamDescriptor() = default;

ZENAPI Descriptor::Descriptor() = default;
ZENAPI Descriptor::Descriptor(
  std::vector<std::string> const &inputs,
  std::vector<std::string> const &outputs,
  std::vector<ParamDescriptor> const &params,
  std::vector<std::string> const &categories)
  : inputs(inputs), outputs(outputs), params(params), categories(categories) {
    this->inputs.push_back("SRC");
    this->inputs.push_back("COND");
    this->outputs.push_back("DST");
}

ZENAPI std::string Descriptor::serialize() const {
  std::string res = "";
  res += "(" + join_str(inputs, ",") + ")";
  res += "(" + join_str(outputs, ",") + ")";
  std::vector<std::string> paramStrs;
  for (auto const &[type, name, defl] : params) {
      paramStrs.push_back(type + ":" + name + ":" + defl);
  }
  res += "(" + join_str(paramStrs, ",") + ")";
  res += "(" + join_str(categories, ",") + ")";
  return res;
}


} // namespace zen