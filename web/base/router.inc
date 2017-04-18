<?php
/**
 *
 * @author amderbar
 *
 */
class Router
{
    private $path_prefix;
    private $get_routing = array();
    private $post_routing = array();

    /**
     *
     * @param string $path_prefix
     */
    public function __construct(?string $path_prefix = null)
    {
        $this->path_prefix = $path_prefix;
    }

    /**
     *
     * @param string $path
     * @param unknown $action
     */
    public function whenGet(string $path, $action) :void
    {
        $this->get_routing[$this->getPrefix() . $path] = $action;
    }

    /**
     *
     * @param string $path
     * @param unknown $action
     */
    public function whenPost(string $path, $action) :void
    {
        $this->get_routing[$this->getPrefix() . $path] = $action;
    }

    /**
     *
     * @param string $path
     * @param unknown $action
     */
    public function whenAny(string $path, $action) :void
    {
        $this->get_routing[$this->getPrefix() . $path] = $action;
        $this->post_routing[$this->getPrefix() . $path] = $action;
    }

    /**
     *
     * @return string
     */
    public function getPrefix(): string
    {
        return $this->path_prefix ?? '';
    }

    /**
     *
     * @param string $path
     * @return array
     */
    public function getGetAction(string $path):array
    {
        return $this->getAction($path, $this->get_routing);
    }

    /**
     *
     * @param string $path
     */
    public function getPostAction(string $path)
    {
        return $this->getAction($path, $this->post_routing);
    }

    /**
     *
     * @param string $path
     * @param array $routing
     * @return array
     */
    private function getAction (string $path, array $routing):array
    {
        foreach ($routing as $key => $action) {
            preg_match_all(':\{([^/]+)\}:', $key, $keys, PREG_SET_ORDER);
            $regex = preg_replace(':\\\{[^/]+\\\}:', '([^/]+)', preg_quote($key));
            if (preg_match(":\A{$regex}/?\Z:", $path, $params)) {
                unset($params[0]);
                return [$action, array_combine(array_column($keys, 1), $params)];
            }
        }
        return [null, null];
    }
}
