<template>
    <table class="data-table" id="def-tbl">
        <thead>
            <tr>
                <th id="add-col">
                    <button type="button" title="列追加" id="add-column" @click="appendColumn">+</button>
                </th>
                <th>No列</th>
                <th is="table-editor-th" v-for="(col, id) in column_config"
                    v-if="col.key != 'id'"
                    :key="id"
                    :is-only="column_config.length < 3"
                    @click="dropColumn(id, $event)"></th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <th>列名</th>
                <td>No.</td>
                <td v-for="(col, id) in column_config" v-if="col.key != 'id'" :key="id">
                    <input type="text"
                        :name="`def_cols[${id}][col_name]`"
                        v-model="column_config[id].col_name"
                        required>
                </td>
            </tr>
            <tr>
                <th>入力形式</th>
                <td>--</td>
                <td v-for="(col, id) in column_config" v-if="col.key != 'id'" :key="id">
                    <select :name="`def_cols[${id}][form_type]`"
                        v-model="column_config[id].type">
                        <option v-for="(label, type) in formTypes"
                            :key="type"
                            :value="type"
                        >{{ label }}</option>
                    </select>
                    <div name="step" v-show="isNumeric[id]" v-cloak>
                        <label>刻み幅:<input type="number"
                            value="1"
                            :disabled="!isNumeric[id]"
                            :name="`def_cols[${id}][step]`"
                            v-model="column_config[id].step"
                        ></label>
                    </div>
                    <div name="max" v-show="isNumeric[id]" v-cloak>
                        <label>最大値:<input type="number"
                            :disabled="!isNumeric[id]"
                            :name="`def_cols[${id}][max]`"
                            v-model="column_config[id].max"
                        ></label>
                    </div>
                    <div name="min" v-show="isNumeric[id]" v-cloak>
                        <label>最小値:<input type="number"
                            :disabled="!isNumeric[id]"
                            :name="`def_cols[${id}][min]`"
                            v-model="column_config[id].min"
                        ></label>
                    </div>
                    <div name="multi" v-show="isMultipleEnable[id] || isMustMultiple[id]" v-cloak>
                        <label><input type="checkbox"
                            value="1"
                            :disabled="!isMultipleEnable[id] || isMustMultiple[id]"
                            :name="`def_cols[${id}][multiple]`"
                            v-model="column_config[id].multiple"
                        ><input type="hidden"
                            value="1"
                            :name="`def_cols[${id}][multiple]`"
                            v-show="isMustMultiple[id]"
                        >複数選択をする</label>
                    </div>
                    <div name="ref" v-show="isForginEnable[id]" v-cloak>
                        <label>選択肢<br v-show="isExistRefDest">
                            <select :name="`def_cols[${id}][ref_dest]`"
                                :disabled="!isForginEnable[id]"
                                v-if="isExistRefDest"
                                v-model="column_config[id].ref_dest">
                                <optgroup label="This table" v-if="column_config.length > 2">
                                    <option v-for="(ref, jd) in column_config"
                                        v-if="!['id', col.key].includes(ref.key)"
                                        :key="ref.key"
                                        :value="- jd">{{ ref.col_name || '列' + jd }}</option>
                                </optgroup>
                            </select>
                            <span v-if="!isExistRefDest">:候補がありません。</span>
                        </label>
                    </div>
                </td>
            </tr>
            <tr>
                <th>初期値</th>
                <td>自動</td>
                <td v-for="(col, id) in column_config"
                    v-if="col.key != 'id'"
                    :key="id">
                    <data-editor-form
                        :key="column_config[id].key"
                        :type="column_config[id].type"
                        :name="`def_cols[${id}][default]`"
                        :options="column_config[id].ref_data || {}"
                        v-model="column_config[id].default"
                    ></data-editor-form>
                </td>
            </tr>
            <tr>
                <th>一意</th>
                <td><input type="checkbox" value="1" disabled checked></td>
                <td v-for="(col, id) in column_config"
                    :key="id"
                    v-if="col.key != 'id'">
                    <input type="checkbox"
                        :name="`def_cols[${id}][uniq]`"
                        value="1"
                        v-model="column_config[id].uniq">
                </td>
            </tr>
            <tr>
                <th>非Null</th>
                <td><input type="checkbox" value="1" disabled checked></td>
                <td v-for="(col, id) in column_config"
                    :key="id"
                    v-if="col.key != 'id'">
                    <input type="checkbox"
                        :name="`def_cols[${id}][not_null]`"
                        value="1"
                        :disabled="isMustNotNull[id]"
                        v-model="column_config[id].not_null">
                    <input type="hidden"
                        :name="`def_cols[${id}][not_null]`"
                        value="1"
                        v-if="isMustNotNull[id]">
                </td>
            </tr>
        </tbody>
    </table>
</template>

<script>
    export default {
        mounted() {
            this.$emit('input', this.column_config);
        },
        props: {
            tblMeta: Array,
            formTypes: Object,
            forginColumns: Array,
        },
        data() {
            return {
                column_config: this.tblMeta,
            };
        },
        computed: {
            scaffold() {
                let new_obj = {};
                Object.keys(this.tblMeta.slice(-1)[0]).forEach(key => { new_obj[key] = null; });
                new_obj['type'] = this.tblMeta.slice(-1)[0].type;
                return new_obj;
            },
            default_data() {
                let default_data = {};
                this.column_config.forEach(conf => {
                    default_data[conf.key] = conf.default;
                });
                return default_data;
            },
            isExistRefDest() {
                return (this.forginColumns.map(forginTbl => forginTbl.length).reduce((carry, num) => carry + num, 0) > 1)
                    || (this.column_config.length > 2);
            },
            isNumeric() {
                return this.column_config.map(conf => {
                    switch (conf.type) {
                        case 'numlist':
                        case 'number':
                        case 'range':
                            return true;
                        default:
                            return false;
                    }
                });
            },
            isMultipleEnable() {
                return this.column_config.map(conf => {
                    switch (conf.type) {
                        case 'select':
                        case 'multicheck':
                            return true;
                        default:
                            return false;
                    }
                });
            },
            isForginEnable() {
                return this.column_config.map(conf => {
                    switch (conf.type) {
                        case 'listext':
                        case 'numlist':
                        case 'select':
                        case 'radio':
                        case 'multicheck':
                            return true;
                        default:
                            return false;
                    }
                });
            },
            isMustMultiple() {
                return this.column_config.map(conf => {
                    switch (conf.type) {
                        case 'multicheck':
                            conf.multiple = true;
                            return true;
                        default:
                            return false;
                    }
                });
            },
            isMustNotNull() {
                return this.column_config.map(conf => {
                    switch (conf.type) {
                        case 'range':
                        case 'color':
                            conf.not_null = true;
                            return true;
                        default:
                            return false;
                    }
                });
            }
        },
        methods: {
            appendColumn() {
                let newKey = (function generateNewKey() {
                    let trialKey = 'c' + Math.random().toString(36).slice(-8);
                    return this.column_config.some(col => col.key == trialKey) ? generateNewKey() : trialKey;
                }.bind(this))();
                this.column_config.push($.extend({}, this.scaffold, {key: newKey}));
            },
            dropColumn(key, event) {
                let eve_target = event.currentTarget || event.target;
                let text = $(eve_target).find('label').text();
                let index = $('.col-h').index(eve_target) + 1;
                if (confirm(`${text}${index}を削除してもよろしいですか？`)) {
                    this.column_config.splice(key, 1);
                }
            }
        },
        watch: {
            column_config() {
                this.$emit('input', this.column_config);
            }
        }
    }
</script>
