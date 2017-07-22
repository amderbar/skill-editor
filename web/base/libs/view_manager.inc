<?php
/**
 * the super class for servlets.
 */
class View
{
    private static $focus;
    private static $html_parts;

    /**
     *
     */
    public static function obStart(string $name) :void
    {
        self::$focus = $name;
        ob_start();
    }

    /**
     *
     */
    public static function obEnd() :void
    {
        echo ob_get_clean();
        self::$focus = null;
    }

    /**
     *
     */
    public static function obStore() :void
    {
        self::$html_parts[self::$focus] = ob_get_clean();
        self::$focus = null;
    }

    /**
     *
     */
    public static function obAppend() :void
    {
        self::$html_parts[self::$focus] .= ob_get_clean();
        self::$focus = null;
    }

    /**
     *
     */
    public static function yield(string $name) :void
    {
        echo self::$html_parts[$name];
    }

    /**
     *
     */
    public static function include(string $dest, array $args = []) :void
    {
        extract($args);
        if (isset($REQ_SCOPE)) {
            extract($REQ_SCOPE);
        }
        include($dest);
    }
}
