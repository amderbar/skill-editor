<?php
/**
 * 
 */
class EditorArea {
    private $href;
    private $proj_id;
    private $tmpl_root = 'view/templates/';
    private $tab_list = null;

    function __construct($url,$proj_id) {
        $this->href = htmlentities($url).'?id='.htmlentities($proj_id);
        $this->proj_id = intval($proj_id);
    }

    public function makePage($tabs,$tmpls,$tmpl_map,$data_list) {
        $this->makeTabs($tabs);
        echo '<form action="'.$this->href.'" method="POST" id="editorArea">'.PHP_EOL;
        foreach ($tabs as $tbl_num => $tbl_name) {
            $tmpl_name = null;
            if (isset($tmpl_map[$tbl_name])) {
                $tmpl_name = $this->tmpl_root . sprintf('proj%03d/',$this->proj_id).$tmpls[$tmpl_map[$tbl_name]];
            }
            $this->makePain($tbl_num,$tmpl_name,$data_list[$tbl_name]);
        }
        echo '</form>'.PHP_EOL;
        $this->makeFooter();
    }

    public function makeTabs($tabs) {
        $this->tab_list = $tabs;
        echo '<div class="header">'.PHP_EOL;
        echo '<ul class="editor-tabs">'.PHP_EOL;
        foreach ($tabs as $tbl_num => $tbl_name) {
            echo '<li><a href="'.$this->href.'#tab'.htmlentities($tbl_num).'" onclick="changeTab(this);">'.htmlentities($tbl_name).'</a></li>'.PHP_EOL;
        }
        echo '</ul>'.PHP_EOL;
        echo '</div>'.PHP_EOL;
    }

    public function makePain($tab_num,$tmpl_name,$data_list) {
        if (! $this->tab_list) {
            return;
        }
        // 以下は現状SNTRPG_Skills専用
        echo '<div id="tab'.htmlentities($tab_num).'" class="tab-page">'.PHP_EOL;
        FormHelper::makeDatalist($this->tab_list[$tab_num],$data_list);
        if ($tmpl_name) {
            $tmpl = file_get_contents(full_path($tmpl_name));
            echo '<ol>'.PHP_EOL;
            foreach ($data_list as $data_row) {
                $tmpl_row = $tmpl;
                foreach ($data_row as $key => $value) {
                    if ($key == 'icon') {
                        $value = 'img/'.$value;
                    }
                    $tmpl_row = str_replace('{'.$key.'}',htmlentities($value),$tmpl_row);
                }
                echo '<li>' . $tmpl_row . '</li>'.PHP_EOL;
            }
            echo '</ol>'.PHP_EOL;
        } else {
            $tmpl_name = $this->tmpl_root . 'default_template.php';
            include($tmpl_name);
        }
        echo '</div>'.PHP_EOL;
    }

    public function makeFooter() {
        echo '<input value="新規レコード" type="button"/>'.PHP_EOL;
        return;
    }
}
?>