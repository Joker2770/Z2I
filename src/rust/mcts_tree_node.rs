use std::sync::atomic::{AtomicI32, AtomicUsize, Ordering};
use std::sync::{Arc, Mutex, Weak};

#[derive(Clone)]
pub struct TreeNode {
    inner: Arc<Mutex<TreeNodeInner>>,
}

pub struct TreeNodeInner {
    parent: Option<Weak<Mutex<TreeNodeInner>>>,
    children: Vec<Option<TreeNode>>,
    is_leaf: bool,
    n_visited: AtomicUsize,
    p_sa: f64,
    q_sa: f64,
    virtual_loss: AtomicI32,
}

impl TreeNode {
    pub fn new(parent: Option<Weak<Mutex<TreeNodeInner>>>, p_sa: f64, action_size: usize) -> Self {
        Self {
            inner: Arc::new(Mutex::new(TreeNodeInner {
                parent,
                children: vec![None; action_size],
                is_leaf: true,
                n_visited: AtomicUsize::new(0),
                p_sa,
                q_sa: 0.0,
                virtual_loss: AtomicI32::new(0),
            })),
        }
    }

    pub fn select(&self, c_puct: f64, c_virtual_loss: f64) -> Option<usize> {
        let child_candidates = {
            let guard = self.inner.lock().unwrap();
            let sum_n_visited = guard.n_visited.load(Ordering::Acquire).saturating_add(1);
            guard
                .children
                .iter()
                .enumerate()
                .filter_map(|(index, child)| {
                    child
                        .as_ref()
                        .map(|node| (index, node.clone(), sum_n_visited))
                })
                .collect::<Vec<_>>()
        };

        let mut best_move = None;
        let mut best_value = f64::NEG_INFINITY;

        for (index, node, sum_n_visited) in child_candidates {
            let value = node.get_value(c_puct, c_virtual_loss, sum_n_visited);
            if value > best_value {
                best_value = value;
                best_move = Some(index);
            }
        }

        if let Some(index) = best_move {
            if let Some(node) = self.child(index) {
                node.inner
                    .lock()
                    .unwrap()
                    .virtual_loss
                    .fetch_add(1, Ordering::AcqRel);
            }
        }

        best_move
    }

    pub fn expand(&self, action_priors: &[f64]) {
        let mut guard = self.inner.lock().unwrap();
        if !guard.is_leaf {
            return;
        }

        let action_size = guard.children.len();
        for (index, &prior) in action_priors.iter().enumerate().take(action_size) {
            if prior <= f64::EPSILON {
                continue;
            }
            let child = TreeNode::new(Some(Arc::downgrade(&self.inner)), prior, action_size);
            guard.children[index] = Some(child);
        }

        guard.is_leaf = false;
    }

    pub fn backup(&self, value: f64) {
        if let Some(parent) = self.parent() {
            parent.backup(-value);
        }

        {
            let guard = self.inner.lock().unwrap();
            guard.virtual_loss.fetch_sub(1, Ordering::AcqRel);
        }

        let mut guard = self.inner.lock().unwrap();
        let n_visited = guard.n_visited.load(Ordering::Acquire);
        let updated_q = (n_visited as f64 * guard.q_sa + value) / (n_visited as f64 + 1.0);
        guard.n_visited.fetch_add(1, Ordering::AcqRel);
        guard.q_sa = updated_q;
    }

    pub fn get_value(&self, c_puct: f64, c_virtual_loss: f64, sum_n_visited: usize) -> f64 {
        let guard = self.inner.lock().unwrap();
        let n_visited = guard.n_visited.load(Ordering::Acquire);
        let u = c_puct * guard.p_sa * (sum_n_visited as f64).sqrt() / (1.0 + n_visited as f64);
        let virtual_loss = c_virtual_loss * guard.virtual_loss.load(Ordering::Acquire) as f64;

        if n_visited == 0 {
            u
        } else {
            let q_sa = guard.q_sa;
            u + (q_sa * n_visited as f64 - virtual_loss) / n_visited as f64
        }
    }

    pub fn is_leaf(&self) -> bool {
        self.inner.lock().unwrap().is_leaf
    }

    pub fn parent(&self) -> Option<TreeNode> {
        let guard = self.inner.lock().unwrap();
        guard
            .parent
            .as_ref()
            .and_then(|parent| parent.upgrade())
            .map(|inner| TreeNode { inner })
    }

    pub fn child(&self, index: usize) -> Option<TreeNode> {
        self.inner
            .lock()
            .unwrap()
            .children
            .get(index)
            .and_then(|child| child.clone())
    }

    pub fn child_count(&self) -> usize {
        self.inner.lock().unwrap().children.len()
    }

    pub fn visits(&self) -> usize {
        self.inner.lock().unwrap().n_visited.load(Ordering::Acquire)
    }

    pub fn value(&self) -> f64 {
        self.inner.lock().unwrap().q_sa
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn expand_creates_children_for_legal_moves() {
        let root = TreeNode::new(None, 1.0, 3);
        let priors = vec![0.0, 0.5, 0.25];

        root.expand(&priors);

        assert!(!root.is_leaf());
        assert!(root.child(0).is_none());
        assert!(root.child(1).is_some());
        assert!(root.child(2).is_some());
    }

    #[test]
    fn backup_updates_visit_count_and_value() {
        let root = TreeNode::new(None, 1.0, 2);
        let child = TreeNode::new(Some(Arc::downgrade(&root.inner)), 0.5, 2);
        root.inner.lock().unwrap().children[0] = Some(child.clone());

        child.backup(0.8);

        assert_eq!(child.visits(), 1);
        assert!((child.value() - 0.8).abs() < 1e-12);
    }
}
